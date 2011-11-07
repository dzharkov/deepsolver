
#ifndef FIXME_INDEX_CORE_H
#define FIXME_INDEX_CORE_H

#include"AbstractWarningHandler.h"

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
  std::string repoName;
  char compressionType;//Must be exactly signed for error value indications;
  char formatType;//Must be exactly signed for error value indications;
}; //class RepoIndexParams;


//FIXME:basic exception;
class IndexCoreException 
{
public:
  IndexCoreException() {}
  IndexCoreException(const std::string& message):
    m_message(message) {}

  virtual ~IndexCoreException() {}

public:
  std::string getMessage() const
  {
    return m_message;
  }

private:
  const std::string m_message;
}; //class IndexCoreException;

class IndexCore
{
public:
  IndexCore(AbstractWarningHandler& warningHandler):
    m_warningHandler(warningHandler) {}

public:
  void build(const std::string& topDir, const std::string& arch, const RepoIndexParams& params);

private:
  void processRpms(const std::string& path);
  void writeInfoFile(const std::string& fileName, const RepoIndexParams& params);

private:
  AbstractWarningHandler& m_warningHandler;
}; //class IndexCore;

#endif //FIXME_INDEX_CORE_H;
