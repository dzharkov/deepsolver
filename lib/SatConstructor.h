
#ifndef DEPSOLVER_SAT_CONSTRUCTOR_H
#define DEPSOLVER_SAT_CONSTRUCTOR_H

#include"PackageScope.h"
#include"sat/Sat.h"

class SatConstructor
{
public:
  SatConstructor(const PackageScope& scope)
    : m_scope(scope) {}

  ~SatConstructor() {}

public:
  void construct(const VarIdVector& strongToInstall,
		 const VarIdVector& strongToRemove,
		 Sat& sat) const;

private:
  const PackageScope& m_scope;
}; //class SatConstructor;

#endif //DEPSOLVER_SAT_CONSTRUCTOR_H;
