
#ifndef FIXME_REPO_INDEX_TEXT_FORMAT_WRITER_H
#define FIXME_REPO_INDEX_TEXT_FORMAT_WRITER_H

#include"PkgFile.h"
#include"NamedPkgRel.h"
#include"AbstractConsoleMessages.h"

/**\brief The class to write repo index data in text format
 *
 * This is the main class for writing repository index data in text
 * format. It must be used by IndexCore class only and not purposed for
 * direct using by library client applications. By the way it performs
 * provides filtering with the options used during the invocation and
 * saves provide resolving data. To be memory-efficient this class
 * creates and then removes one or more temporary files. It means that
 * directory for repository index must have more free disk space than
 * just for index data itself.
 *
 * The work consists of three steps: initialization, package data
 * collecting and commit. On initialization only temporary file for
 * package data storing is created . On every add operation during the
 * collecting step all required information about single package file is
 * written to this temporary file. Main work is done on commit step. This
 * step makes provides filtering and building provide resolving table. 
 *
 * Provide resolving table shows what packages can be used for every
 * provide entry dependency. Building this information during repository
 * index creation can significantly reduce time used for package
 * installation and removing operations. 
 *
 * There are two options for provide filtering:register the provide entry
 * if corresponding require dependency is present and filter provides by
 * directories list. The second case is actual only for provides as files
 * from the file list of the package. The filtering mode by require
 * dependencies includes dependencies from collected packages and in
 * additional from optional string list.
 * \sa IndexCore RepoIndexInfoFile
 * 
 */
class RepoIndexTextFormatWriter
{
public:
  RepoIndexTextFormatWriter(AbstractConsoleMessages& console, const std::string& dir, bool filterProvidesByRequires, const StringSet& additionalRequires, const StringList& filterProvidesByDirs);

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
  void additionalPhase();

private:
  AbstractConsoleMessages& m_console;
  const std::string m_dir;
  const std::string m_rpmsFileName, m_providesFileName;
  std::string m_tmpFileName;
  StringToIntMap m_provideMap;
  ProvideResolvingItemVector m_resolvingItems;
  SizeVector m_resolvingData;
  std::ofstream m_os;
  const bool m_filterProvidesByRequires;
  const StringSet& m_additionalRequires;
  const StringList& m_filterProvidesByDirs;
  StringSet m_requiresSet;
}; //class RepoIndexTextFormatWriter;

#endif //FIXME_REPO_INDEX_TEXT_FORMAT_WRITER_H;
