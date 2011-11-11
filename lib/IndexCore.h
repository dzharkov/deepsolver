
#ifndef FIXME_INDEX_CORE_H
#define FIXME_INDEX_CORE_H

#include"AbstractWarningHandler.h"
#include"IndexCoreException.h"

class RepoIndexParams
{
public:
  enum {
    CompressionTypeNone = 0,
    CompressionTypeGzip = 1
  };

  enum {
    FormatTypeText = 1,//The single currently supported;
    FormatTypeBinary = 2
  };

  RepoIndexParams()
    : compressionType(DEFAULT_REPO_INDEX_COMPRESSION_TYPE), formatType(DEFAULT_REPO_INDEX_FORMAT_TYPE) {}//Real values are defined in DefaultValues.h;

public:
  char compressionType;//Must be exactly signed for error value indications;
  char formatType;//Must be exactly signed for error value indications;
}; //class RepoIndexParams;

class IndexCore
{
public:
  IndexCore(AbstractWarningHandler& warningHandler):
    m_warningHandler(warningHandler) {}

public:
    void build(const std::string& topDir, const std::string& arch, const RepoIndexParams& params, const StringToStringMap& userParams);

private:
    void processRpms(const std::string& indexDir, const std::string& pkgDir, const RepoIndexParams& params);
  void writeInfoFile(const std::string& fileName, const RepoIndexParams& params, const StringToStringMap& userParams);

private:
  AbstractWarningHandler& m_warningHandler;
}; //class IndexCore;

#endif //FIXME_INDEX_CORE_H;
