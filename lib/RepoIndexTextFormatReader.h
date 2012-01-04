
#ifndef DEPSOLVER_REPO_INDEX_TEXT_FORMAT_READER_H
#define DEPSOLVER_REPO_INDEX_TEXT_FORMAT_READER_H

#include"PkgFile.h"
#include"TextFiles.h"

class RepoIndexTextFormatReader
{
public:
  RepoIndexTextFormatReader(const std::string& dir, char compressionType)
    : m_dir(dir), m_compressionType(compressionType) {}

  ~RepoIndexTextFormatReader() {}

public:
  void openRpms();
  void openSrpms();
  void openProvides();
  bool readPackage(PkgFile& pkgFile);
  bool readProvides(std::string& provideName, StringList& providers);

private:
  const std::string& m_dir;
  const char m_compressionType;
  std::auto_ptr<AbstractTextFileReader> m_reader;

}; //class RepoIndexTextFormatReader;

#endif //DEPSOLVER_REPO_INDEX_TEXT_FORMAT_READER_H;
