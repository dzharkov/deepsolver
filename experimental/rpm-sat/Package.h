
#ifndef __APT_NG_PACKAGE_H__
#define __APT_NG_PACKAGE_H__

#include<assert.h>
#include<vector>
#include<list>
#include<set>
#include<string>

class PkgRel
{
public:
  enum {
    None = 0,
    Less = 1,
    LessOrEqual = 2,
    Equal = 3,
    GreaterOrEqual = 4,
    Greater = 5
  };

 PkgRel(): versionRel(None) {}
  PkgRel(const std::string& n): name(n), versionRel(None) {}
  PkgRel(const std::string& n, const std::string& v, char vr): name(n), version(v), versionRel(vr) {}

public:
  bool canBeEqual() const
  {
    assert(versionRel != None);
    return versionRel == Equal || versionRel == LessOrEqual || versionRel == GreaterOrEqual;
  }

  bool canBeLess() const
  {
    assert(versionRel != None);
    return versionRel == Less || versionRel == LessOrEqual;
  }

  bool canBeGreater() const
  {
    assert(versionRel != None);
    return versionRel == Greater || versionRel == GreaterOrEqual;
  }

public:
  std::string name, version;
  char versionRel;
}; //class PkgRel;

typedef std::list<PkgRel>  PkgRelList;
typedef std::vector<PkgRel> PkgRelVector;

//This class contains only data required for experiments and some general package information;
class Package
{
public:
  Package(): epoch(0) {}

public:
  std::string getFullVersion() const;

public:
  std::string name, version, release, arch, url, packager, summary, description;
  int epoch;
  PkgRelVector requires, conflicts, provides, obsoletes;
}; //class Package;

typedef std::vector<Package> PackageVector;
typedef std::list<Package> PackageList;

typedef size_t PackageId;
#define BAD_PACKAGE_ID (PackageId)-1

typedef std::vector<PackageId> PackageIdVector;
typedef std::set<PackageId> PackageIdSet;

std::ostream& operator <<(std::ostream& s, const Package& p);
std::ostream& operator <<(std::ostream& s, const PkgRel& p);

#endif //__APT_NG_PACKAGE_H__;
