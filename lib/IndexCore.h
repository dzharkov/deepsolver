
#ifndef FIXME_INDEX_CORE_H
#define FIXME_INDEX_CORE_H

class RepoIndexParams
{
public:
  enum {
    CompressionTypeNone = 0;
    CompressionTypeGzip = 1;
  };

  enum {
    FormatTypeText = 1;//The single currently supported;
    FormatTypeBinary = 2;
  };

  RepoIndexParams()
    : compressionType(DEFAULT_REPO_INDEX_COMPRESSION_TYPE), formatType(DEFAULT_REPO_INDEX_FORMAT_TYPE) {}//Real values are defined in DefaultValues.h;

public:
  std::string name;
  char compressionType;
  char formatType;
}; //class RepoIndexParams;

class IndexCore
{
public:
  IndexCore() {}

public:
  void build(const std::string& topDir, const std::string& arch, const RepoIndexParams& params);
}; //class IndexCore;

#ifndef //FIXME_INDEX_CORE_H;
