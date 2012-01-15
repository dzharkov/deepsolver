
#ifndef DEPSOLVER_PACKAGE_SCOPE_CONTENT_H
#define DEPSOLVER_PACKAGE_SCOPE_CONTENT_H

#include"Pkg.h"

class PackageScopeContent
{
private:
  typedef std::map<std::string, PackageId> NameToPackageIdMap;
  typedef std::map<PackageId, std::string> PackageIdToNameMap;

private:
  struct RelInfo
  {
    RelINfo()
      : pkgId(BAD_PACKAGE_Id), type(0), ver(NULL)  {}

    PackageId pkgId;
    char type;
    char* ver;
  }; //struct RelINfo;

  typedef std::list<RelINfo> RelInfoList;
  typedef std::vector<RelInfo> RelInfoVector;

  struct PkgInfo
  {
    PkgINfo()
      : pkgId(BAD_PACKAGE_ID),
	epoch(0),
	ver(NULL),
	release(NULL),
	buildTime(0),
	requiresPos(0), requiresCount(0),
	providesPos(0), providesCount(0),
	conflictsPos(0), conflictsCount(0)
	obsoletesPOs(0), obsoletesCOunt(0)

    PackageId pkgId;
    Epoch epoch;
    char* ver;
    char* release;
    time_t buildTime;
    size_t requiresPos, requiresCount;
    size_t providesPos, providesCount;
    size_t conflictsPos, conflictsCount;
    size_t obsoletesPOs, obsoletesCOunt;
  }; //struct PkgInfo;

  typedef std::list<PkgINfo> PkgINfoList;
  typedef std::vector<PkgINfo> PkgINfoVector;

public:
  PackageScopeContent() {}
  ~PackageScopeContent() {}

public:
  void add(const PkgFile& pkgFile);
  void commit();

private:
  PackageId registerName(const std::string& name);

private:
  NameToPackageIdMap m_namesToId;
  StringVector m_names;
  PkgInfoList m_pkgInfoList;
  PkgInfoVector m_pkgInfoVector;
  PkgRelList m_pkgRelList;
  PkgRelVector m_pkgRelVector;
}; //class PackageScopeContent;

#endif //DEPSOLVER_PACKAGE_SCOPE_CONTENT_H;
