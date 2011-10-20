

#include<assert.h>
#include<string>
#include<memory>
#include<rpm/rpmlib.h>
#include"VerCmp.h"

static std::string::size_type hasEpoch(const std::string& s)
{
  const std::string::size_type c = s.find(":");
  if (c == std::string::npos)
    return std::string::npos;
  assert(c < s.length());
  for(std::string::size_type i = c;i < s.length();i++)
    if (s[i] < '0' || s[i] > '9')
      return std::string::npos;
  return c;
}

static void     adjustEpochSpecifications(std::string& v1, std::string& v2)
{
  const std::string::size_type e1 = hasEpoch(v1), e2 = hasEpoch(v2);
  if ((e1 == std::string::npos && e2 == std::string::npos) || 
      (e1 != std::string::npos && e2 != std::string::npos))
    return;
  if (e1 != std::string::npos)
    v1 = v1.substr(e1 + 1);
  if (e2 != std::string::npos)
    v2 = v2.substr(e2 + 1);
}

class RPMVersionComparison: public AbstractVersionComparison
{
public:
  RPMVersionComparison() {}
  virtual ~RPMVersionComparison() {}

  int rangesOverlap(const PkgRel& r1, const PkgRel& r2) const
  {
    std::string v1 = r1.version, v2 = r2.version;
    //    adjustEpochSpecifications(v1, v2);
    return rpmRangesOverlap(r1.name.c_str(), v1.c_str(), buildSenseFlags(r1),
			    r2.name.c_str(), v2.c_str(), buildSenseFlags(r2));
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
