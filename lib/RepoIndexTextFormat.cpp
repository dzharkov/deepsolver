
#include"basic-header.h"
#include"RepoIndexTextFormat.h"
#include"IndexCoreException.h"

#define TMP_FILE "tmp_packages_data"

#define NAME_STR "name="
#define PROVIDES_STR "provides:"

RepoIndexTextFormat::RepoIndexTextFormat(const std::string& dir)
  : m_dir(dir),
    m_rpmsFileName(concatUnixPath(dir, REPO_INDEX_RPMS_DATA_FILE)),
    m_providesFileName(concatUnixPath(dir, REPO_INDEX_PROVIDES_DATA_FILE)),
    m_tmpFileName(concatUnixPath(dir, TMP_FILE))
{
}

void RepoIndexTextFormat::init()
{
  m_os.open(m_tmpFileName.c_str());
  if (!m_os)
    INDEX_CORE_STOP("Error creating temporary file \'" + m_tmpFileName + "\'");
}

void RepoIndexTextFormat::add(const PkgFile& pkgFile,
			      const NamedPkgRelList& provides,
			      const NamedPkgRelList& requires,
			      const NamedPkgRelList& conflicts,
			      const NamedPkgRelList& obsoletes,
			      const StringList& fileList)
{
  m_os << "[" << File::baseName(pkgFile.fileName) << "]" << std::endl;
  m_os << "name=" << pkgFile.name << std::endl;
  m_os << "version=" << pkgFile.version << std::endl;
  for(NamedPkgRelList::const_iterator it = provides.begin();it != provides.end();it++)
    {
      m_os << PROVIDES_STR << (*it) << std::endl;
      firstProvideReg(pkgFile.name, it->pkgName);
    }
    m_os << std::endl;
}

void RepoIndexTextFormat::commit()
{
  m_os.close();
  prepareResolvingData();
  secondPhase();
  writeProvideResolvingData();
  File::unlink(m_tmpFileName);
}

void RepoIndexTextFormat::firstProvideReg(const std::string& pkgName, const std::string& provideName)
{
  StringToIntMap::iterator it;
  it = m_provideMap.find(pkgName);
  if (it == m_provideMap.end())
    m_provideMap.insert(StringToIntMap::value_type(pkgName, 0));
  it = m_provideMap.find(provideName);
  if (it == m_provideMap.end())
    m_provideMap.insert(StringToIntMap::value_type(provideName, 1)); else
    it->second++;
}

void RepoIndexTextFormat::prepareResolvingData()
{
  m_resolvingItems.reserve(m_provideMap.size());
  for(StringToIntMap::const_iterator it = m_provideMap.begin();it != m_provideMap.end();it++)
    m_resolvingItems.push_back(ProvideResolvingItem(it->first, it->second));
  m_provideMap.clear();
  //Normally m_provideMap must be iterated by the increasing order of name, but we run sorting one more time to be completely sure;
  std::sort(m_resolvingItems.begin(), m_resolvingItems.end());
  const size_t totalCount = fillProvideResolvingItemsPos(m_resolvingItems);
  m_resolvingData.resize(totalCount);
  for(SizeVector::size_type i = 0;i < m_resolvingData.size();i++)
    m_resolvingData[i] = (size_t) -1;
}

RepoIndexTextFormat::ProvideResolvingItemVector::size_type RepoIndexTextFormat::findProvideResolvingItem(const std::string& name)
{
  assert(!m_resolvingItems.empty());
  ProvideResolvingItemVector::size_type l = 0, r = m_resolvingItems.size();
  while(l < r)
    {
      const ProvideResolvingItemVector::size_type middle = (l + r) / 2;
      assert(middle < m_resolvingItems.size());
      if (m_resolvingItems[middle].name == name)
	return middle;
      if (m_resolvingItems[middle].name > name)
	r = middle; else
	l = middle;
    }
  assert(0);
  return 0;//Just to reduce warning messages;
}

size_t RepoIndexTextFormat::fillProvideResolvingItemsPos(ProvideResolvingItemVector& v)
{
  size_t c = 0;
  for(ProvideResolvingItemVector::size_type i = 0;i < v.size();i++)
    {
      v[i].pos = c;
      c += v[i].count;
    }
  return c;
}

void RepoIndexTextFormat::secondPhase()
{
  const std::string tmpFileName = concatUnixPath(m_dir, TMP_FILE);

  std::ifstream is(tmpFileName.c_str());
  if (!is)
    INDEX_CORE_STOP("Could not open temporary file \'" + tmpFileName + "\' for second phase processing");
  std::ofstream os(m_rpmsFileName.c_str());
  if (!os)
    INDEX_CORE_STOP("Could not create file with package headers information \'" + m_rpmsFileName + "\'");
  std::string name;
  while (1)
    {
      std::string line;
      std::getline(is, line);
      if (!is)
	break;
      std::string tail;
      if (stringBegins(line, NAME_STR, tail))
	name = tail;
      if (!stringBegins(line, PROVIDES_STR, tail))
	{
	  os << line << std::endl;
	  continue;
	}
      const std::string::size_type spacePos = tail.find(" ");
      const std::string provideName = spacePos != std::string::npos?tail.substr(0, spacePos):tail;
      assert(!provideName.empty());
      assert(!name.empty());
      secondProvideReg(name, provideName);
      //FIXME:provide filtering;
      os << line << std::endl;
    } //while(1);
  writeProvideResolvingData();
}

void RepoIndexTextFormat::secondProvideReg(const std::string& pkgName, const std::string& provideName)
{
  //  std::cout << "secondProvideReg(" << pkgName << ", " << provideName << ")" << std::endl;
  const ProvideResolvingItemVector::size_type itemIndex = findProvideResolvingItem(provideName);
  assert(itemIndex < m_resolvingItems.size());
  const ProvideResolvingItem& item = m_resolvingItems[itemIndex];
  const ProvideResolvingItemVector::size_type pkgIndex = findProvideResolvingItem(pkgName);
  assert(pkgIndex < m_resolvingItems.size());
  SizeVector::size_type i = item.pos;;
  while(i < item.pos + item.count && m_resolvingData[i] != (size_t)-1)
    i++;
  assert(i <item.pos + item.count);
  assert(i < m_resolvingData.size());
  m_resolvingData[i] = pkgIndex;
}

void RepoIndexTextFormat::writeProvideResolvingData()
{
  std::ofstream os(m_providesFileName.c_str());
  if (!os)
    INDEX_CORE_STOP("Could not create file for provides data \'" + m_providesFileName + "\'");
  for(ProvideResolvingItemVector::size_type i = 0;i < m_resolvingItems.size();i++)
    {
      const ProvideResolvingItem& item = m_resolvingItems[i];
      if (item.count == 0)
	continue;
      os << "[" << item.name << "]" << std::endl;
      for(SizeVector::size_type k = item.pos;k < item.pos + item.count && m_resolvingData[k] != (size_t)-1;k++)
	{
	  assert(m_resolvingData[k] < m_resolvingItems.size());
	  os << m_resolvingItems[m_resolvingData[k]].name << std::endl;
	}
      os << std::endl;
    }
}
