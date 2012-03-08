
#ifndef DEEPSOLVER_REQUIRE_FILTER_H
#define DEEPSOLVER_REQUIRE_FILTER_H

#include"RepoIndexTextFormatWriter.h"

class RequireFilter: public AbstractRequireFilter
{
public:
  RequireFilter() {}
  virtual ~RequireFilter() {}

public:
  void load(const std::string& fileName);
  bool excludeRequire(const std::string& requireEntry) const;

private:
  StringVector m_requiresToExclude;
}; //class RequireFilter;

#endif //DEEPSOLVER_REQUIRE_FILTER_H;
