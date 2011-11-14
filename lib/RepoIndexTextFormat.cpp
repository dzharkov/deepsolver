
#include"basic-header.h"
#include"RepoIndexTextFormat.h"
#include"IndexCoreException.h"

#define TMP_FILE "tmp_file1"

struct ProvideResolvingItem
{
  public:
  ProvideResolvingItem(const std::string& n, size_t c)
    : name(n), count(c), pos(0) {}

public:
  bool operator <(const ProvideResolvingItem& i) const
  {
    return name < i.name;
  }

  bool operator >(const ProvideResolvingItem& i) const
  {
    return name > i.name;
  }

public:
  std::string name;
  size_t count, pos;
}; //struct ProvideResolvingItem;

typedef std::vector<ProvideResolvingItem> ProvideResolvingItemVector;

static ProvideResolvingItemVector::size_type findProvideResolvingItem(const ProvideResolvingItemVector& v, const std::string& name)
{
  assert(!v.empty());
  ProvideResolvingItemVector::size_type l = 0, r = v.size();
  while(l < r)
    {
      const ProvideResolvingItemVector::size_type middle = (l + r) / 2;
      assert(middle < v.size());
      if (v[middle].name == name)
	return middle;
      if (v[middle].name > name)
	r = middle; else
	l = middle;
    }
  assert(0);
  return 0;//Just to reduce warning messages;
}

static size_t fillProvideResolvingItemsPos(ProvideResolvingItemVector& v)
{
  size_t c = 0;
  for(ProvideResolvingItemVector::size_type i = 0;i < v.size();i++)
    {
      v[i].pos = c;
      c += v[i].count;
    }
  return c;
}

void RepoIndexTextFormat::init()
{
  const std::string tmpFileName = concatUnixPath(m_dir, TMP_FILE);
  m_os.open(tmpFileName.c_str());
  if (!m_os)
    INDEX_CORE_STOP("Error creating temporary file \'" + tmpFileName + "\'");
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
      m_os << "provides:" << (*it) << std::endl;
      //FIXME:      firstProvideReg(pkgFile.name, it->pkgName);
    }
    m_os << std::endl;
}

void RepoIndexTextFormat::commit()
{
  m_os.close();

  std::cout << "proba" << std::endl;
  while(1);


  ProvideResolvingItemVector resolvingItems;
  resolvingItems.reserve(m_provideMap.size());
  for(StringToIntMap::const_iterator it = m_provideMap.begin();it != m_provideMap.end();it++)
    resolvingItems.push_back(ProvideResolvingItem(it->first, it->second));
  m_provideMap.clear();
  //Normally m_provideMap must be iterated by the increasing order of name, but we run sorting one more time to be completely sure;
  std::sort(resolvingItems.begin(), resolvingItems.end());
  const size_t totalCount = fillProvideResolvingItemsPos(resolvingItems);
  SizeVector resolvingData;
  resolvingData.resize(totalCount);
  std::cout << "ready!" << std::endl;
    std::cout << "totalCount=" << totalCount << std::endl;
    while(1);
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
