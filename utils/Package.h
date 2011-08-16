
#ifndef __APT_NG_PACKAGE_H__
#define __APT_NG_PACKAGE_H__

#include<vector>
#include<list>
#include<string>
#include<rpm/rpmlib.h>//FIXME:Just for int_32 type;

class PkgRel
{
public:
  enum {
    Less = 1,
    LessOrEqual = 2,
    Equal = 3,
    GreaterOrEqual = 4,
    Greater = 5
  };

  PkgRel(): flags(0), versionRel(0) {}
  PkgRel(const std::string& n): name(n), flags(0), versionRel(0) {}
  PkgRel(const std::string& n, const std::string& v): name(n), version(v), flags(0), versionRel(0) {}
  PkgRel(const std::string& n, const std::string& v, int_32 f): name(n), version(v), flags(f), versionRel(0) {}

  std::string name, version;
  int_32 flags;
  char versionRel;
}; //class PkgRel;

typedef std::list<PkgRel>  PkgRelList;
typedef std::vector<PkgRel> PkgRelVector;

//This class contains only data required for experiments and some general package information;
class Package
{
public:
  Package(): epoch(0) {}

  std::string name, version, release, arch, url, packager, summary, description;
  int_32 epoch;
  PkgRelVector requires, conflicts, provides, obsoletes;
}; //class Package;

std::ostream& operator <<(std::ostream& s, const Package& p);
std::ostream& operator <<(std::ostream& s, const PkgRel& p);

#endif //__APT_NG_PACKAGE_H__;
