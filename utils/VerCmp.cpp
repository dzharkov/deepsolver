

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

  int compare(const std::string& s1, const std::string& s2) const
  {
    assert(!s1.empty() && !s2.empty());
  return rpmvercmp(s1.c_str(), s2.c_str());
  }
}; //class RPMVersionComparison;

std::auto_ptr<AbstractVersionComparison> createVersionComparison(int type)
{
  assert(type == VersionComparisonRPM);
  return std::auto_ptr<AbstractVersionComparison>(new RPMVersionComparison());
}
