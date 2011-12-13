
#include"depsolver.h"
#include"IndexCore.h"
#include"RepoIndexInfoFile.h"
#include"RepoIndexTextFormatWriter.h"
#include"rpm/RpmFile.h"
#include"rpm/RpmFileHeaderReader.h"

void IndexCore::collectRequires(const std::string& dirName, StringSet& res) 
{
  m_console.msg() << "Collecting additional requires from " << dirName << "...";
  std::auto_ptr<Directory::Iterator> it = Directory::enumerate(dirName);
  while(it->moveNext())
    {
      if (it->getName() == "." || it->getName() == "..")
	continue;
      if (!checkExtension(it->getName(), ".rpm"))
	continue;
      NamedPkgRelList requires;
      RpmFileHeaderReader reader;
      reader.load(it->getFullPath());
      reader.fillRequires(requires);
      for(NamedPkgRelList::const_iterator it = requires.begin();it != requires.end();it++)
	{
	  StringSet::iterator resIt = res.find(it->pkgName);
	  if (resIt == res.end())
	    res.insert(it->pkgName);
	}
    }
  m_console.msg() << " " << res.size() << " found!" << std::endl;
}

void IndexCore::collectRequiresFromDirs(const StringList& dirs, StringSet& res)
{
  res.clear();
  for(StringList::const_iterator it = dirs.begin();it != dirs.end();it++)
    collectRequires(*it, res);
}

void IndexCore::build(const RepoIndexParams& params)
{
  const std::string archDir = concatUnixPath(params.topDir, params.arch);
  const std::string indexDir = concatUnixPath(archDir, REPO_INDEX_DIR);
  const std::string infoFile = concatUnixPath(indexDir, REPO_INDEX_INFO_FILE);
  Directory::ensureExists(indexDir);
  writeInfoFile(infoFile, params);
  processPackages(indexDir,
		  concatUnixPath(archDir, REPO_RPMS_DIR_NAME),
		  concatUnixPath(archDir, REPO_SRPMS_DIR_NAME),
		  params);
}

void IndexCore::processPackages(const std::string& indexDir, const std::string& rpmsDir, const std::string& srpmsDir, const RepoIndexParams& params)
{
  assert(params.formatType == RepoIndexParams::FormatTypeText);//FIXME:binary format support;
  StringSet additionalRequires;
  if (params.provideFilteringByRequires)
    collectRequiresFromDirs(params.provideFilterDirs, additionalRequires);
  RepoIndexTextFormatWriter handler(params, m_console, indexDir, additionalRequires);
  //Binary packages;
  handler.initBinary();
  m_console.msg() << "Looking through " << rpmsDir << " to pick packages for repository index...";
  std::auto_ptr<Directory::Iterator> it = Directory::enumerate(rpmsDir);
  size_t count = 0;
  while(it->moveNext())
    {
      if (it->getName() == "." || it->getName() == "..")
	continue;
      if (!checkExtension(it->getName(), ".rpm"))
	continue;
      PkgFile pkgFile;
      NamedPkgRelList provides, requires, conflicts, obsoletes;
      StringList files;
      readRpmPkgFile(it->getFullPath(), pkgFile, provides, requires, conflicts, obsoletes, files);
      handler.addBinary(pkgFile, provides, requires, conflicts, obsoletes, files);
      count++;
    }
  m_console.msg() << " picked up " << count << " binary packages!" << std::endl;
  handler.commitBinary();
  //Source packages;
  m_console.msg() << "Looking through " << srpmsDir << " to pick source packages...";
  it = Directory::enumerate(srpmsDir);
  count = 0;
  while(it->moveNext())
    {
      if (it->getName() == "." || it->getName() == "..")
	continue;
      if (!checkExtension(it->getName(), ".src.rpm"))
	continue;
      PkgFile pkgFile;
      NamedPkgRelList provides, requires, conflicts, obsoletes;
      StringList files;
      readRpmPkgFile(it->getFullPath(), pkgFile, provides, requires, conflicts, obsoletes, files);
      handler.addSource(pkgFile);
      count++;
    }
  m_console.msg() << " picked up " << count << " source packages!" << std::endl;
  handler.commitSource();
}

void IndexCore::writeInfoFile(const std::string& fileName, const RepoIndexParams& params)
{
  RepoIndexInfoFile infoFile;
  switch(params.compressionType)
    {
    case RepoIndexParams::CompressionTypeNone:
      infoFile.setCompressionType("none");
      break;
    case RepoIndexParams::CompressionTypeGzip:
      infoFile.setCompressionType("gzip");
      break;
    default:
      assert(0);
    }; //switch(compressionType);
  switch(params.formatType)
    {
    case RepoIndexParams::FormatTypeText:
      infoFile.setFormatType("text");
      break;
    case RepoIndexParams::FormatTypeBinary:
      infoFile.setFormatType("binary");
      break;
    default:
      assert(0);
    }; //switch(formatType);
  infoFile.setFormatVersion(PACKAGE_VERSION);
  infoFile.setMd5sumFile(REPO_INDEX_MD5SUM_FILE);
  for(StringToStringMap::const_iterator it = params.userParams.begin();it != params.userParams.end();it++)
    infoFile.addUserParam(it->first, it->second);
  std::string errorMessage;
  StringList warningMessages;
  const bool res = infoFile.write(fileName, errorMessage, warningMessages);
  for(StringList::const_iterator it = warningMessages.begin();it != warningMessages.end();it++)
    m_warningHandler.onWarning(*it);
  if (!res)
    INDEX_CORE_STOP(errorMessage);
  m_console.msg() << "Created repository index info file " << fileName << std::endl;
}
