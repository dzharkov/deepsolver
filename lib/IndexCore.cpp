
#include"basic-header.h"
#include"IndexCore.h"
#include"RepoIndexInfoFile.h"


void IndexCore::build(const std::string& topDir, const std::string& arch, const RepoIndexParams& params)
{
  RepoINdexInfoFile inforFile;
  assert(hasNonSpaces(params.repoName));
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
  //FIXME:
}
