
#ifndef DEEPSOLVER_REPO_INDEX_TEXT_FORMAT_READER_H
#define DEEPSOLVER_REPO_INDEX_TEXT_FORMAT_READER_H

#include"Pkg.h"
#include"TextFiles.h"

class RepoIndexTextFormatReader
{
public:
  RepoIndexTextFormatReader(const std::string& dir, char compressionType)
    : m_dir(dir), m_compressionType(compressionType) {}

  ~RepoIndexTextFormatReader() {}

public:
  void openPackagesFile();
  void openSourcePackagesFile();
  void openProvidesFile();
  bool readPackage(PkgFile& pkgFile);
  bool readSourcePackage(PkgFile& pkgFile);
  bool readProvides(std::string& provideName, StringVector& providers);

private:
  const std::string& m_dir;
  const char m_compressionType;
  std::auto_ptr<AbstractTextFileReader> m_reader;
  std::string m_lastSectionHeader;
  bool m_noMoreData;
}; //class RepoIndexTextFormatReader;

#endif //DEEPSOLVER_REPO_INDEX_TEXT_FORMAT_READER_H;
