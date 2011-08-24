

#include<assert.h>
#include<string>
#include<memory>
#include<rpm/rpmlib.h>
#include"VerCmp.h"

class RPMVersionComparison: public AbstractVersionComparison
{
public:
  RPMVersionComparison() {}
  virtual ~RPMVersionComparison() {}

  int rangesOverlap(const PkgRel& r1, const PkgRel& r2) const
  {
    return rpmRangesOverlap(r1.name.c_str(), r1.version.c_str(), buildSenseFlags(r1),
			    r2.name.c_str(), r2.version.c_str(), buildSenseFlags(r2));
  }

private:
  int buildSenseFlags(const PkgRel& pkgRel) const
  {
    int value = 0;
    if (pkgRel.canBeEqual())
      value |= RPMSENSE_EQUAL;
    if (pkgRel.canBeLess())
      value |= RPMSENSE_LESS;
    if (pkgRel.canBeGreater())
      value |= RPMSENSE_GREATER;
    return value;
  }
}; //class RPMVersionComparison;

std::auto_ptr<AbstractVersionComparison> createVersionComparison(int type)
{
  assert(type == VersionComparisonRPM);
  return std::auto_ptr<AbstractVersionComparison>(new RPMVersionComparison());
}
