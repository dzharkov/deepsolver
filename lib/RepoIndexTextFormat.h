
#ifndef FIXME_REPO_INDEX_TEXT_FORMAT_H
#define FIXME_REPO_INDEX_TEXT_FORMAT_H

#include"PkgFile.h"
#include"NamedPkgRel.h"

class RepoIndexTextFormat
{
public:
  RepoIndexTextFormat(const std::string& dir);

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
  struct ProvideResolvingItem
  {
  public:
    ProvideResolvingItem(const std::string& n, size_t c)
      : name(n), count(c), pos(0) {}

  public:
    bool operator <(const ProvideResolvingItem& i) const
    {
      return name < i.name;
    }

    bool operator >(const ProvideResolvingItem& i) const
    {
      return name > i.name;
    }

  public:
    std::string name;
    size_t count, pos;
  }; //struct ProvideResolvingItem;

private:
  typedef std::vector<ProvideResolvingItem> ProvideResolvingItemVector;
  typedef std::map<std::string, size_t> StringToIntMap;

private:
  void firstProvideReg(const std::string& pkgName, const std::string& provideName);
  void secondProvideReg(const std::string& pkgName, const std::string& provideName);
  void prepareResolvingData();
  size_t fillProvideResolvingItemsPos(ProvideResolvingItemVector& v);
  ProvideResolvingItemVector::size_type findProvideResolvingItem(const std::string& name);
  void writeProvideResolvingData();
  void secondPhase();

private:
  const std::string m_dir;
  const std::string m_rpmsFileName, m_providesFileName, m_tmpFileName;
  StringToIntMap m_provideMap;
  ProvideResolvingItemVector m_resolvingItems;
  SizeVector m_resolvingData;
  std::ofstream m_os;
}; //class RepoIndexTextFormat ;

#endif //FIXME_REPO_INDEX_TEXT_FORMAT_H;
