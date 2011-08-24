
#ifndef __APT_NG_VERCMP_H__
#define __APT_NG_VERCMP_H__

#include<memory>
#include"Package.h"

class AbstractVersionComparison
{
public:
  virtual ~AbstractVersionComparison() {}

  //  virtual int compare(const std::string& s1, const std::string& s2) const = 0;
  virtual int rangesOverlap(const PkgRel& r1, const PkgRel& r2) const = 0;
}; //class AbstractVersionComparison;

enum {VersionComparisonRPM = 0};
std::auto_ptr<AbstractVersionComparison> createVersionComparison(int type);

#endif //__APT_NG_VERCMP_H__;
