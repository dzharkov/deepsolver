
#ifndef DEPSOLVER_SAT_CONSTRUCTOR_H
#define DEPSOLVER_SAT_CONSTRUCTOR_H

#include"UserTaskItem.h"
#include "AbstractProvideResolver.h"
#include"Sat.h"

class SatConstructor
{
public:
  SatConstructor(const AbstractProvideResolver& provideResolver)
    : m_provideResolver(provideResolver) {}

public:
  void construct(const UserTask& task, Sat& sat) const;

private:
  const AbstractProvideResolver& m_provideResolver;
}; //class SatConstructor;

#endif //DEPSOLVER_SAT_CONSTRUCTOR_H;
