
#include"deepsolver.h"
#include<rpm/rpmlib.h>

int rpmVerCmp(const std::string& ver1, const std::string& ver2)
{
  return rpmvercmp(ver1.c_str(), ver2.c_str());
}

static int buildSenseFlags(const VersionCond& c)
{
  int value = 0;
  if (c.isEqual())
    value |= RPMSENSE_EQUAL;
  if (c.isLess())
    value |= RPMSENSE_LESS;
  if (c.isGreater())
    value |= RPMSENSE_GREATER;
  return value;
}

int rpmVerOverlap(const VersionCond& v1, const VersionCond& v2)
{
  return rpmRangesOverlap("", v1.version.c_str(), buildSenseFlags(v1),
			  "", v2.version.c_str(), buildSenseFlags(v2));
}
