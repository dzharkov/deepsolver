

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

static void splitVersionRelease(const std::string& s, std::string& version, std::string& release)
{
  version.erase();
  release.erase();
  if (s.empty())
    return;
  const std::string::size_type p = s.find("-");
  if (p == std::string::npos)
    {
      version = s;
      return;
    }
  version = s.substr(0, p);
  release = s.substr(p + 1);
}

int VersionReleaseComparison::compare(const std::string& s1, const std::string& s2) const
{
  assert(!s1.empty() && !s2.empty());
  std::string v1, v2, r1, r2;
  splitVersionRelease(s1, v1, r1);
  splitVersionRelease(s2, v2, r2);
  assert(m_verCmp.get() != NULL);
  const int res = m_verCmp->compare(v1, v2);
  if (res != 0)
    return res; else 
    return m_verCmp->compare(r1, r2);
}

std::auto_ptr<AbstractVersionComparison> createVersionComparison(int type)
{
  assert(type == VersionComparisonRPM);
  return std::auto_ptr<AbstractVersionComparison>(new RPMVersionComparison());
}
