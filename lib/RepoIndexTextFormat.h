
#ifndef FIXME_REPO_INDEX_TEXT_FORMAT_H
#define FIXME_REPO_INDEX_TEXT_FORMAT_H

#include"PkgFile.h"
#include"NamedPkgRel.h"

class RepoIndexTextFormat
{
public:
  RepoIndexTextFormat(const std::string& dir)
    : m_dir(dir) {}

public:
  void init();

  void add(const PkgFile& pkgFile,
	   const NamedPkgRelList& provides,
	   const NamedPkgRelList& requires,
	   const NamedPkgRelList& conflicts,
	   const NamedPkgRelList& obsoletes,
	   const StringList& fileList);

  void commit();

private:
  void firstProvideReg(const std::string& pkgName, const std::string& provideName);

private:
  typedef std::map<std::string, size_t> StringToIntMap;

private:
  const std::string m_dir;
  StringToIntMap m_provideMap;
  std::ofstream m_os;
}; //class RepoIndexTextFormat ;

#endif //FIXME_REPO_INDEX_TEXT_FORMAT_H;
