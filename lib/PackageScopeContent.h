
#ifndef DEPSOLVER_PACKAGE_SCOPE_CONTENT_H
#define DEPSOLVER_PACKAGE_SCOPE_CONTENT_H

#include"Pkg.h"

class PackageScopeContent
{
private:
  typedef std::map<std::string, PackageId> NameToPackageIdMap;
  typedef std::map<PackageId, std::string> PackageIdToNameMap;

public:
  struct RelInfo
  {
    RelInfo()
      : pkgId(BAD_PACKAGE_ID), type(0), ver(NULL)  {}

    PackageId pkgId;
    VerDirection type;
    char* ver;
  }; //struct RelINfo;

  typedef std::list<RelInfo> RelInfoList;
  typedef std::vector<RelInfo> RelInfoVector;

  struct PkgInfo
  {
    PkgInfo()
      : pkgId(BAD_PACKAGE_ID),
	epoch(0),
	ver(NULL),
	release(NULL),
	buildTime(0),
	requiresPos(0), requiresCount(0),
	providesPos(0), providesCount(0),
      conflictsPos(0), conflictsCount(0),
      obsoletesPos(0), obsoletesCount(0) {}

    PackageId pkgId;
    Epoch epoch;
    char* ver;
    char* release;
    time_t buildTime;
    size_t requiresPos, requiresCount;
    size_t providesPos, providesCount;
    size_t conflictsPos, conflictsCount;
    size_t obsoletesPos, obsoletesCount;
  }; //struct PkgInfo;

  typedef std::list<PkgInfo> PkgInfoList;
  typedef std::vector<PkgInfo> PkgInfoVector;

public:
  PackageScopeContent() {}
  ~PackageScopeContent() {}

public:
  void add(const PkgFile& pkgFile);
  bool checkName(const std::string& name) const;
  PackageId strToPackageId(const std::string& name) const;
  std::string packageIdToStr(PackageId packageId) const;
  const PkgInfoVector& getPkgs() const;
  const RelInfoVector& getRels() const;

private:
  void processRels(const NamedPkgRelVector& rels, size_t& pos, size_t& count);
  PackageId registerName(const std::string& name);

private:
  NameToPackageIdMap m_namesToId;
  StringVector m_names;
  PkgInfoVector m_pkgInfoVector;
  RelInfoVector m_relInfoVector;
}; //class PackageScopeContent;

#endif //DEPSOLVER_PACKAGE_SCOPE_CONTENT_H;
