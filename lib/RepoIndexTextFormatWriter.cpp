//FIXME:change log;

#include"depsolver.h"
#include"RepoIndexTextFormatWriter.h"
#include"IndexCoreException.h"

#define TMP_FILE "tmp_packages_data1"
#define TMP_FILE_ADDITIONAL "tmp_packages_data2"

#define NAME_STR "name="
#define EPOCH_STR "epoch="
#define VERSION_STR "version="
#define RELEASE_STR "release="
#define ARCH_STR "arch="
#define URL_STR "URL="
#define LICENSE_STR "license="
#define PACKAGER_STR "packager="
#define SUMMARY_STr "summary="
#define DESCRIPTION_STR "descr="

#define PROVIDES_STR "provides:"
#define REQUIRES_STR "requires:"
#define CONFLICTS_STR "conflicts:"
#define OBSOLETES_STR "obsoletes:"

static std::string getPkgRelName(const std::string& line)
{
  //Name is stored at the beginning of line until first space without previously used backslash;
  std::string res;
  for(std::string::size_type i = 0;i < line.length();i++)
    {
      if (line[i] == '\\')
	{
	  i++;
	  if (i < line.length())
	    res += line[i]; else 
	    return res + "\\";
	  continue;
	}
      if (line[i] == ' ')
	return res;
      res += line[i];
    } //for();
  return res;
}

static std::string saveNamedPkgRel(const NamedPkgRel& r)
{
  std::ostringstream s;
  std::string name;
  for(std::string::size_type i = 0;i < r.pkgName.length();i++)
    {
      if (r.pkgName[i] == ' ' || r.pkgName[i] == '\\')
	name += "\\";
      name += r.pkgName[i];
    }
  s << name;
  if (r.ver.empty())
    return s.str();
  const bool less = r.type & NamedPkgRel::Less, equals = r.type & NamedPkgRel::Equals, greater = r.type & NamedPkgRel::Greater;
  assert(!less || !greater);
  std::string t;
  if (less)
    t += "<";
  if (equals)
    t += "=";
  if (greater)
    t += ">";
  s << " " << t << " " << r.ver;
  return s.str();
}

static std::string saveFileName(const std::string& fileName)
{
  std::string s;
  for(std::string::size_type i = 0;i < fileName.length();i++)
    {
      if (fileName[i] == ' ' || fileName[i] == '\\')
	s += "\\";
      s += fileName[i];
    }
  return s;
}

static std::string encodeDescr(const std::string& s)
{
  std::string r;
  for(std::string::size_type i = 0;i < s.length();i++)
    {
      switch(s[i])
	{
	case '\\':
	  r += "\\\\";
	  break;
	case '\n':
	  r += "\\n";
	  break;
	case '\r':
	  continue;
	case '\t':
	  r += "\\t";
	  break;
	default:
	  r += s[i];
	}; //switch(s[i]);
    }
  return r;
}

static bool fileFromDirs(const std::string& fileName, const StringList& dirs)
{
  std::string tail;
  for(StringList::const_iterator it = dirs.begin();it != dirs.end();it++)
    {
      if (stringBegins(fileName, *it, tail))
	return 1;
    }
  return 0;
}

RepoIndexTextFormatWriter::RepoIndexTextFormatWriter(AbstractConsoleMessages& console, const std::string& dir, bool filterProvidesByRequires, const StringSet& additionalRequires, const StringList& filterProvidesByDirs)
  : m_console(console),
    m_dir(dir),
    m_rpmsFileName(concatUnixPath(dir, REPO_INDEX_RPMS_DATA_FILE)),
    m_providesFileName(concatUnixPath(dir, REPO_INDEX_PROVIDES_DATA_FILE)),
    m_tmpFileName(concatUnixPath(dir, TMP_FILE)),
    m_filterProvidesByRequires(filterProvidesByRequires),
    m_additionalRequires(additionalRequires),
    m_filterProvidesByDirs(filterProvidesByDirs)
{
}

void RepoIndexTextFormatWriter::init()
{
  m_tmpFile = createTextFileWriter(TextFileStd, m_tmpFileName);
}

void RepoIndexTextFormatWriter::add(const PkgFile& pkgFile,
			      const NamedPkgRelList& provides,
			      const NamedPkgRelList& requires,
			      const NamedPkgRelList& conflicts,
			      const NamedPkgRelList& obsoletes,
			      const StringList& fileList)
{
  m_tmpFile->writeLine("[" + File::baseName(pkgFile.fileName) + "]");
  m_tmpFile->writeLine(NAME_STR + pkgFile.name);
  m_tmpFile->writeLine(EPOCH_STR + pkgFile.epoch);
  m_tmpFile->writeLine(VERSION_STR + pkgFile.version);
  m_tmpFile->writeLine(RELEASE_STR + pkgFile.release);
  m_tmpFile->writeLine(ARCH_STR + pkgFile.arch);
  m_tmpFile->writeLine(URL_STR + pkgFile.url);
  m_tmpFile->writeLine(LICENSE_STR + pkgFile.license);
  m_tmpFile->writeLine(PACKAGER_STR + pkgFile.packager);
  m_tmpFile->writeLine(SUMMARY_STr + pkgFile.summary);
  m_tmpFile->writeLine(DESCRIPTION_STR + encodeDescr(pkgFile.description));
  for(NamedPkgRelList::const_iterator it = provides.begin();it != provides.end();it++)
    {
      /*
       * The following operation must be done in both cases: in filtering by
       * requires mode and without filtering. If there is no filtering we just
       * saving all provides, if filtering is enabled we will proceed real
       * filtering on additional phase.
       */
      m_tmpFile->writeLine(PROVIDES_STR + saveNamedPkgRel(*it));
      //But registration must be done only if we have no additional phase ;
      if (!m_filterProvidesByRequires)
	firstProvideReg(pkgFile.name, it->pkgName);
    }
  for(StringList::const_iterator it = fileList.begin();it != fileList.end();it++)
    /*
     * If filtering by requires is enabled we are writing all possible
     * provides to filter them on additional phase. If filterProvidesByDirs
     * string list is empty it means filtering by directories is disabled and
     * we also must write current file as provides. If filtering by
     * directories is enabled we are writing file as provides only if its
     * directory presents in directory list.
     */
    if (m_filterProvidesByRequires || m_filterProvidesByDirs.empty() || fileFromDirs(*it, m_filterProvidesByDirs))
      {
	m_tmpFIle->writeLine(PROVIDES_STR + saveFileName(*it));
	//But we are performing registration only if there is no additional phase ;
	if (!m_filterProvidesByRequires)
	  firstProvideReg(pkgFile.name, *it);
      }
  for(NamedPkgRelList::const_iterator it = requires.begin();it != requires.end();it++)
    {
      m_tmpFile->writeLine(REQUIRES_STR + saveNamedPkgRel(*it));
      if (m_filterProvidesByRequires)
	m_requiresSet.insert(it->pkgName);
    }
  for(NamedPkgRelList::const_iterator it = conflicts.begin();it != conflicts.end();it++)
    m_tmpFile->writeLine(CONFLICTS_STR + saveNamedPkgRel(*it));
  for(NamedPkgRelList::const_iterator it = obsoletes.begin();it != obsoletes.end();it++)
    m_tmpFile->writeLine(OBSOLETES_STR + saveNamedPkgRel(*it));
  m_tmpFile->writeLine("");
}

void RepoIndexTextFormatWriter::commit()
{
  m_tmpFile->close();
  assert(m_tmpFileName == concatUnixPath(m_dir, TMP_FILE));//KILLME:
  if (m_filterProvidesByRequires)
    {
      m_console.msg() << "Saved " << m_requiresSet.size() << " requires from repository for provides filtering, running additional step...";
      additionalPhase();
      m_console.msg() << " provides filtering completed!" << std::endl;
    }
  prepareResolvingData();
  secondPhase();
  writeProvideResolvingData();
  File::unlink(m_tmpFileName);
}

void RepoIndexTextFormatWriter::additionalPhase()
{
  assert(m_provideMap.empty());
  assert(m_resolvingItems.empty());
  assert(m_resolvingData.empty());
  assert(m_filterProvidesByRequires);
  const std::string inputFileName = m_tmpFileName, outputFileName = concatUnixPath(m_dir, TMP_FILE_ADDITIONAL);
  m_tmpFileName = outputFileName;//Changing name of a file to be processed on the second phase;
  std::auto_ptr<AbstractTextFileReader> inputFile = create TextFileReader(TextFileStd, inputFileName);
 std:;auto_ptr<AbstractTextFileOutput> outputFile = createTextFileWriter(TextFileStd, outputFileName);
  std::string name, line;
  while (inputFile->readLine(line))
    {
      std::string tail;
      if (stringBegins(line, NAME_STR, tail))
	{
	  name = tail;
	  outputFile->writeLine(line);
	  continue;
	}
      if (!stringBegins(line, PROVIDES_STR, tail))
	{
	  outputFile->writeLine(line);
	  continue;
	}
      const std::string provideName = getPkgRelName(tail);
      assert(!provideName.empty());
      if (m_requiresSet.find(provideName) != m_requiresSet.end() || m_additionalRequires.find(provideName) != m_additionalRequires.end() ||
	  (!m_filterProvidesByDirs.empty() && fileFromDirs(provideName, m_filterProvidesByDirs)))
	{
	  outputFile->writeLine(line);
	  firstProvideReg(name, provideName);
	}
    } //while();
  inputFile->close();
  outputFile->close();
  File::unlink(inputFileName);
}

void RepoIndexTextFormatWriter::firstProvideReg(const std::string& pkgName, const std::string& provideName)
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

void RepoIndexTextFormatWriter::prepareResolvingData()
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

void RepoIndexTextFormatWriter::secondPhase()
{
  assert(!m_tmpFileName.empty());
  std::ifstream is(m_tmpFileName.c_str());
  if (!is)
    INDEX_CORE_STOP("Could not open temporary file \'" + m_tmpFileName + "\' for second phase processing");
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
      const std::string provideName = getPkgRelName(tail);
      assert(!provideName.empty());
      assert(!name.empty());
      secondProvideReg(name, provideName);
      os << line << std::endl;
    } //while(1);
}

void RepoIndexTextFormatWriter::secondProvideReg(const std::string& pkgName, const std::string& provideName)
{
  const ProvideResolvingItemVector::size_type itemIndex = findProvideResolvingItem(provideName);
  assert(itemIndex < m_resolvingItems.size());
  const ProvideResolvingItem& item = m_resolvingItems[itemIndex];
  const ProvideResolvingItemVector::size_type pkgIndex = findProvideResolvingItem(pkgName);
  assert(pkgIndex < m_resolvingItems.size());
  SizeVector::size_type i = item.pos;
  while(i < item.pos + item.count && m_resolvingData[i] != (size_t)-1)
    i++;
  assert(i <item.pos + item.count);
  assert(i < m_resolvingData.size());
  m_resolvingData[i] = pkgIndex;
}

void RepoIndexTextFormatWriter::writeProvideResolvingData()
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

RepoIndexTextFormatWriter::ProvideResolvingItemVector::size_type RepoIndexTextFormatWriter::findProvideResolvingItem(const std::string& name)
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

size_t RepoIndexTextFormatWriter::fillProvideResolvingItemsPos(ProvideResolvingItemVector& v)
{
  size_t c = 0;
  for(ProvideResolvingItemVector::size_type i = 0;i < v.size();i++)
    {
      v[i].pos = c;
      c += v[i].count;
    }
  return c;
}
