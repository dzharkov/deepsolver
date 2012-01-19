//FIXME:change log;

#include"depsolver.h"
#include"RepoIndexTextFormatWriter.h"
#include"IndexCoreException.h"

#define TMP_FILE "tmp_packages_data1"
#define TMP_FILE_ADDITIONAL "tmp_packages_data2"

#define NAME_STR "n="
#define EPOCH_STR "e="
#define VERSION_STR "v="
#define RELEASE_STR "r="
#define ARCH_STR "arch="
#define URL_STR "URL="
#define LICENSE_STR "lic="
#define PACKAGER_STR "pckgr="
#define SUMMARY_STr "summ="
#define DESCRIPTION_STR "descr="
#define SRCRPM_STR "src="
#define BUILDTIME_STR "btime="

#define PROVIDES_STR "p:"
#define REQUIRES_STR "r:"
#define CONFLICTS_STR "c:"
#define OBSOLETES_STR "o:"
#define CHANGELOG_STR "cl:"

static int selectTextFileType(int compressionParam)
{
  switch(compressionParam)
    {
    case RepoIndexParams::CompressionTypeNone:
      return TextFileStd;
    case RepoIndexParams::CompressionTypeGzip:
      return TextFileGZip;
    default:
      assert(0);
    }; //switch();
  return 0;//Just to reduce warning messages;
}

static std::string addCompressionExtension(const std::string& fileName, const RepoIndexParams& params)
{
  if (params.compressionType == RepoIndexParams::CompressionTypeGzip)
    return fileName + ".gz";
  assert(params.compressionType == RepoIndexParams::CompressionTypeNone);
  return fileName;
}

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
  const bool less = r.type & VerLess, equals = r.type & VerEquals, greater = r.type & VerGreater;
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

static std::string encodeMultiline(const std::string& s)
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
	default:
	  r += s[i];
	}; //switch(s[i]);
    }
  return r;
}

static std::string encodeChangeLogEntry(const ChangeLogEntry& entry)
{
  struct tm brTime;
  gmtime_r(&entry.time, &brTime);
  std::ostringstream s;
  s << (brTime.tm_year + 1900) << "-";
  if (brTime.tm_mon < 10)
    s << "0";
  s << (brTime.tm_mon + 1) << "-";
  if (brTime.tm_mday < 10)
    s << "0";
  s << brTime.tm_mday << std::endl;
  s << entry.name << std::endl;
  s << entry.text;
  return encodeMultiline(s.str());
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

RepoIndexTextFormatWriter::RepoIndexTextFormatWriter(const RepoIndexParams& params,
						     AbstractConsoleMessages& console,
						     const std::string& dir,
						     const StringSet& additionalRefs)
  : m_params(params),
    m_console(console),
    m_dir(dir),
    m_rpmsFileName(addCompressionExtension(concatUnixPath(dir, REPO_INDEX_RPMS_DATA_FILE), params)),
    m_srpmsFileName(addCompressionExtension(concatUnixPath(dir, REPO_INDEX_SRPMS_DATA_FILE), params)),
    m_providesFileName(addCompressionExtension(concatUnixPath(dir, REPO_INDEX_PROVIDES_DATA_FILE), params)),
    m_tmpFileName(concatUnixPath(dir, TMP_FILE)),
    m_filterProvidesByRefs(params.provideFilteringByRefs),
    m_additionalRefs(additionalRefs),
    m_filterProvidesByDirs(params.provideFilterDirs)
{
}

void RepoIndexTextFormatWriter::initBinary()
{
  m_tmpFile = createTextFileWriter(TextFileStd, m_tmpFileName);
}

void RepoIndexTextFormatWriter::initSource()
{
  m_srpmsFile = createTextFileWriter(selectTextFileType(m_params.compressionType), m_srpmsFileName);
}

void RepoIndexTextFormatWriter::addBinary(const PkgFile& pkgFile, const StringList& fileList)
{
  m_tmpFile->writeLine("[" + File::baseName(pkgFile.fileName) + "]");
  m_tmpFile->writeLine(NAME_STR + pkgFile.name);
  std::ostringstream epochStr;
  epochStr << EPOCH_STR << pkgFile.epoch;
  m_tmpFile->writeLine(epochStr.str());
  m_tmpFile->writeLine(VERSION_STR + pkgFile.version);
  m_tmpFile->writeLine(RELEASE_STR + pkgFile.release);
  m_tmpFile->writeLine(ARCH_STR + pkgFile.arch);
  m_tmpFile->writeLine(URL_STR + pkgFile.url);
  m_tmpFile->writeLine(LICENSE_STR + pkgFile.license);
  m_tmpFile->writeLine(PACKAGER_STR + pkgFile.packager);
  m_tmpFile->writeLine(SUMMARY_STr + pkgFile.summary);
  m_tmpFile->writeLine(DESCRIPTION_STR + encodeMultiline(pkgFile.description));
  m_tmpFile->writeLine(SRCRPM_STR + pkgFile.srcRpm);
  std::ostringstream bt;
  bt << pkgFile.buildTime;
  m_tmpFile->writeLine(BUILDTIME_STR + bt.str());
  for(NamedPkgRelVector::const_iterator it = pkgFile.provides.begin();it != pkgFile.provides.end();it++)
    {
      /*
       * The following operation must be done in both cases: in filtering by
       * references mode and without filtering. If there is no filtering we just
       * saving all provides, if filtering is enabled we will proceed real
       * filtering on additional phase.
       */
      m_tmpFile->writeLine(PROVIDES_STR + saveNamedPkgRel(*it));
      //But registration must be done only if we have no additional phase ;
      if (!m_filterProvidesByRefs)
	firstProvideReg(pkgFile.name, it->pkgName);
    }
  for(StringList::const_iterator it = fileList.begin();it != fileList.end();it++)
    /*
     * If filtering by references is enabled we are writing all possible
     * provides to filter them on additional phase. If filterProvidesByDirs
     * string list is empty it means filtering by directories is disabled and
     * we also must write current file as provides. If filtering by
     * directories is enabled we are writing file as provides only if its
     * directory presents in directory list.
     */
    if (m_filterProvidesByRefs || m_filterProvidesByDirs.empty() || fileFromDirs(*it, m_filterProvidesByDirs))
      {
	m_tmpFile->writeLine(PROVIDES_STR + saveFileName(*it));
	//But we are performing registration only if there is no additional phase ;
	if (!m_filterProvidesByRefs)
	  firstProvideReg(pkgFile.name, *it);
      }
  for(NamedPkgRelVector::const_iterator it = pkgFile.requires.begin();it != pkgFile.requires.end();it++)
    {
      m_tmpFile->writeLine(REQUIRES_STR + saveNamedPkgRel(*it));
      if (m_filterProvidesByRefs)
	m_refsSet.insert(it->pkgName);
    }
  for(NamedPkgRelVector::const_iterator it = pkgFile.conflicts.begin();it != pkgFile.conflicts.end();it++)
    {
      m_tmpFile->writeLine(CONFLICTS_STR + saveNamedPkgRel(*it));
      if (m_filterProvidesByRefs)
	m_refsSet.insert(it->pkgName);
    }
  for(NamedPkgRelVector::const_iterator it = pkgFile.obsoletes.begin();it != pkgFile.obsoletes.end();it++)
    m_tmpFile->writeLine(OBSOLETES_STR + saveNamedPkgRel(*it));
  if (m_params.changeLogBinary)
    for(ChangeLog::size_type i = 0;i < pkgFile.changeLog.size();i++)
      m_tmpFile->writeLine(CHANGELOG_STR + encodeChangeLogEntry(pkgFile.changeLog[i]));
  m_tmpFile->writeLine("");
}

void RepoIndexTextFormatWriter::addSource(const PkgFile& pkgFile)
{
  m_srpmsFile->writeLine("[" + File::baseName(pkgFile.fileName) + "]");
  m_srpmsFile->writeLine(NAME_STR + pkgFile.name);
  std::ostringstream epochStr;
  epochStr << EPOCH_STR << pkgFile.epoch;
  m_srpmsFile->writeLine(epochStr.str());
  m_srpmsFile->writeLine(VERSION_STR + pkgFile.version);
  m_srpmsFile->writeLine(RELEASE_STR + pkgFile.release);
  //No nned need to write arch for source packages;
  m_srpmsFile->writeLine(URL_STR + pkgFile.url);
  m_srpmsFile->writeLine(LICENSE_STR + pkgFile.license);
  m_srpmsFile->writeLine(PACKAGER_STR + pkgFile.packager);
  m_srpmsFile->writeLine(SUMMARY_STr + pkgFile.summary);
  m_srpmsFile->writeLine(DESCRIPTION_STR + encodeMultiline(pkgFile.description));
  //No need to write src.rpm entry, usually it is empty for source packages;
  if (m_params.changeLogSources)
    for(ChangeLog::size_type i = 0;i < pkgFile.changeLog.size();i++)
      m_srpmsFile->writeLine(CHANGELOG_STR + encodeChangeLogEntry(pkgFile.changeLog[i]));
  m_srpmsFile->writeLine("");
}

void RepoIndexTextFormatWriter::commitBinary()
{
  m_tmpFile->close();
  assert(m_tmpFileName == concatUnixPath(m_dir, TMP_FILE));//KILLME:
  if (m_filterProvidesByRefs)
    {
      m_console.msg() << "Collected " << m_refsSet.size() << " references among picked packages for provides filtering" << std::endl; 
      m_console.msg() << "running additional phase...";
      additionalPhase();
      m_console.msg() << " provides filtering completed!" << std::endl;
    }
  prepareResolvingData();
  m_console.msg() << "" "Writing final binary package index file " << m_rpmsFileName << "...";
  secondPhase();
  m_console.msg() << " OK!" << std::endl;
  m_console.msg() << "Writing provide resolving file " << m_providesFileName << "...";
  writeProvideResolvingData();
  m_console.msg() << " OK!" << std::endl;
  logMsg(LOG_DEBUG, "Removing \'%s\'", m_tmpFileName.c_str());
  File::unlink(m_tmpFileName);
}

void RepoIndexTextFormatWriter::commitSource()
{
  m_srpmsFile->close();
}

void RepoIndexTextFormatWriter::additionalPhase()
{
  assert(m_provideMap.empty());
  assert(m_resolvingItems.empty());
  assert(m_resolvingData.empty());
  assert(m_filterProvidesByRefs);
  const std::string inputFileName = m_tmpFileName, outputFileName = concatUnixPath(m_dir, TMP_FILE_ADDITIONAL);
  m_tmpFileName = outputFileName;//Changing name of a file to be processed on the second phase;
  std::auto_ptr<AbstractTextFileReader> inputFile = createTextFileReader(TextFileStd, inputFileName);
  std::auto_ptr<AbstractTextFileWriter> outputFile = createTextFileWriter(TextFileStd, outputFileName);
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
      if (m_refsSet.find(provideName) != m_refsSet.end() || m_additionalRefs.find(provideName) != m_additionalRefs.end() ||
	  (!m_filterProvidesByDirs.empty() && fileFromDirs(provideName, m_filterProvidesByDirs)))
	{
	  outputFile->writeLine(line);
	  firstProvideReg(name, provideName);
	}
    } //while();
  inputFile->close();
  outputFile->close();
  logMsg(LOG_DEBUG, "Removing \'%s\'", inputFileName.c_str());
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
  std::auto_ptr<AbstractTextFileReader> inputFile = createTextFileReader(TextFileStd, m_tmpFileName);
  std::auto_ptr<AbstractTextFileWriter> outputFile = createTextFileWriter(selectTextFileType(m_params.compressionType), m_rpmsFileName);
  std::string name, line;
  while(inputFile->readLine(line))
    {
      std::string tail;
      if (stringBegins(line, NAME_STR, tail))
	name = tail;
      if (!stringBegins(line, PROVIDES_STR, tail))
	{
	  outputFile->writeLine(line);
	  continue;
	}
      const std::string provideName = getPkgRelName(tail);
      assert(!provideName.empty());
      assert(!name.empty());
      secondProvideReg(name, provideName);
      outputFile->writeLine(line);
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
  std::auto_ptr<AbstractTextFileWriter> file = createTextFileWriter(selectTextFileType(m_params.compressionType), m_providesFileName);
  for(ProvideResolvingItemVector::size_type i = 0;i < m_resolvingItems.size();i++)
    {
      const ProvideResolvingItem& item = m_resolvingItems[i];
      if (item.count == 0)
	continue;
      file->writeLine("[" + item.name + "]");
      for(SizeVector::size_type k = item.pos;k < item.pos + item.count && m_resolvingData[k] != (size_t)-1;k++)
	{
	  assert(m_resolvingData[k] < m_resolvingItems.size());
	  file->writeLine(m_resolvingItems[m_resolvingData[k]].name);
	}
      file->writeLine("");
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
