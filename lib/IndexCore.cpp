
#include"basic-header.h"
#include"IndexCore.h"
#include"RepoIndexInfoFile.h"

#define INDEX_CORE_STOP(x) throw IndexCoreException(x)

void IndexCore::build(const std::string& topDir, const std::string& arch, const RepoIndexParams& params)
{
  const std::string archDir = concatUnixPath(topDir, arch);
  const std::string indexDir = concatUnixPath(archDir, REPO_INDEX_DIR);
  const std::string infoFile = concatUnixPath(indexDir, REPO_INDEX_INFO_FILE);
  std::cout << infoFile << std::endl;
}

void IndexCore::writeInfoFile(const std::string& fileName, const RepoIndexParams& params)
{
  RepoIndexInfoFile infoFile;
  infoFile.setRepoName(params.repoName);
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
    default:
      assert(0);
    }; //switch(formatType);
  //  infoFile.write();

}
