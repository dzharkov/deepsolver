

#include"AttachedRepo.h"

void AttachedRepo::init(const std::string initString)
{
  //FIXME:
}

void AttachedRepo::assignId(AttachedRepoId newValue)
{
  assert(newValue != ATTACHED_REPO_BAD_ID);
  assert(m_id == ATTACHED_REPO_BAD_ID);//This operation must be performed only once;
  m_id = newValue;
}

attachedRepoId AttachedRepo::getId() const
{
  assert(m_id != ATTACHED_REPO_BAD_ID);
  return m_id;
}
