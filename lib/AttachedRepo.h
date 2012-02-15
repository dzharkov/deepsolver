
#ifndef DEEPSOLVER_ATTACHED_REPO_H
#define DEEPSOLVER_ATTACHED_REPO_H

typedef size_t AttachedRepoId;
#define ATTACHED_REPO_BAD_ID (AttachedRepoId)-1

class AttachedRepo
{
public:
 AttachedRepo():
    m_id(ATTACHED_REPO_BAD_ID), m_flags(0) {}

public:
  void init(const std::string initString);
  void assignId(AttachedRepoId newValue);
  attachedRepoId getId() const;

private:
  AttachedRepoId m_id;
  int m_flags;
  std::string m_url;
  std::string m_signId;
}; //class AttachedRepo;

typedef std::list<AttachedRepo*> AttachedRepoList;
typedef std::vector<AttachedRepo*> AttachedRepoVector;

#endif //DEEPSOLVER_ATTACHED_REPO_H;
