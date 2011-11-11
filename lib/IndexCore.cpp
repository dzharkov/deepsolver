
#include"basic-header.h"
#include"IndexCore.h"
#include"RepoIndexInfoFile.h"
#include"RepoIndexTextFormat.h"
#include"rpm/RpmFile.h"

#define INDEX_CORE_STOP(x) throw IndexCoreException(x)

void IndexCore::build(const std::string& topDir, const std::string& arch, const RepoIndexParams& params, const StringToStringMap& userParams)
{
  const std::string archDir = concatUnixPath(topDir, arch);
  const std::string indexDir = concatUnixPath(archDir, REPO_INDEX_DIR);
  const std::string infoFile = concatUnixPath(indexDir, REPO_INDEX_INFO_FILE);
  Directory::ensureExists(indexDir);
  writeInfoFile(infoFile, params, userParams);
  processRpms(indexDir, concatUnixPath(archDir, REPO_RPMS_DIR_NAME), params);
}

void IndexCore::processRpms(const std::string& indexDir, const std::string& pkgDir, const RepoIndexParams& params)
{
  assert(params.formatType == RepoIndexParams::FormatTypeText);//FIXME:binary format support;
  RepoIndexTextFormat handler(indexDir);
  handler.init();
  std::auto_ptr<Directory::Iterator> it = Directory::enumerate(pkgDir);
  while(it->moveNext())
    {
      if (it->getName() == "." || it->getName() == "..")
	continue;
      if (checkExtension(it->getName(), ".rpm"))
	continue;
      PkgFile pkgFile;
      NamedPkgRelList provides, requires, conflicts, obsoletes;
      StringList files;
      readRpmPkgFile(it->getFullPath(), pkgFile, provides, requires, conflicts, obsoletes, files);
      handler.add(pkgFile, provides, requires, conflicts, obsoletes, files);
    }
  handler.commit();
}

void IndexCore::writeInfoFile(const std::string& fileName, const RepoIndexParams& params, const StringToStringMap& userParams)
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
  for(StringToStringMap::const_iterator it = userParams.begin();it != userParams.end();it++)
    infoFile.addUserParam(it->first, it->second);
  std::string errorMessage;
  StringList warningMessages;
  const bool res = infoFile.write(fileName, errorMessage, warningMessages);
  for(StringList::const_iterator it = warningMessages.begin();it != warningMessages.end();it++)
    m_warningHandler.onWarning(*it);
  if (!res)
    INDEX_CORE_STOP(errorMessage);
}
