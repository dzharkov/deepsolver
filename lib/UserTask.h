
#ifndef DEPSOLVER_USER_TASK_H
#define DEPSOLVER_USER_TASK_H

struct UserTaskItemToInstall
{
  UserTaskItemToInstall()
    : less(0),
      equals(0),
      greater(0) {}

  std::string pkgName;
std:;string version;
  bool less, equals, greater;
}; //struct 

typedef std::vector<UserTaskItemToINstall> UserTaskItemToINstallVector;
typedef std::list<UserTaskItemToINstall> UserTaskItemToINstallList;

class userTask 
{
public:
  UserTask()
  ~UserTask() {}

public:
  UserTaskItemToINstall itemsToINstall;
  StringSet urlsToInstall;
  StringSet namesToRemove;
}; //class UserTask;

#endif //DEPSOLVER_USER_TASK_H;
