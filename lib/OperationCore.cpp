
#include"OperationCore.h"

OperationCore::~OperationCore()
{
  destroyRepos();
}

void OperationCore::load()
{
  fillRepos();
  //FIXME:
}

void OperationCore::destroyRepos()
{
  for(AttachedRepoVector::size_type i = 0;i < m_repos.size();i++)
    delete m_repos[i];
  m_repos.clear();
}

void OperationCore::fillRepos()
{
  //FIXME:
  for(AttachedRepoVector::size_type i = 0;i < m_repos.size();i++)
    m_repos[i]->assignId(i);
}
