
#ifndef DEEPSOLVER_OPERATION_CORE_H
#define DEEPSOLVER_OPERATION_CORE_H

#include"ConfigCenter.h"
#include"AttachedRepo.h"

class OperationCore
{
public:
  OperationCore(const ConfigCenter& conf): 
    m_conf(conf)  {}
  virtual ~OperationCore(); 

public:
  void load();

private:
  void destroyRepos();
  void fillRepos();

private:
  const ConfigCenter& m_conf;
  AttachedRepoVector m_repos;
}; //class OperationCore;

#endif //DEEPSOLVER_OPERATION_CORE_H;
