
#ifndef __APT_NG_VERCMP_H__
#define __APT_NG_VERCMP_H__

#include<memory>

class AbstractVersionComparison
{
public:
  virtual ~AbstractVersionComparison() {}

  virtual int compare(const std::string& s1, const std::string& s2) const = 0;
}; //class AbstractVersionComparison;

class VersionReleaseComparison: public AbstractVersionComparison
{
public:
  VersionReleaseComparison(std::auto_ptr<AbstractVersionComparison> verCmp): m_verCmp(verCmp) {}
  virtual ~VersionReleaseComparison() {}

  virtual int compare(const std::string& s1, const std::string& s2) const;

private:
  std::auto_ptr<AbstractVersionComparison> m_verCmp;
}; //class VersionReleaseComparison;

enum {VersionComparisonRPM = 0};
std::auto_ptr<AbstractVersionComparison> createVersionComparison(int type);

#endif //__APT_NG_VERCMP_H__;
