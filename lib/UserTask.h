
#ifndef DEPSOLVER_USER_TASK_H
#define DEPSOLVER_USER_TASK_H

class UserTaskItemToInstall
{
public:
  UserTaskItemToInstall()
    : less(0),
      equals(0),
      greater(0) {}

public:
  std::string makeStr() const
  {
    assert(!less || !greater);
    std::string s = pkgName;
    if (version.empty())
      return s;
    s += " ";
    assert(less || equals || greater);
    if (less && equals)
      s += "less or equals";
    if (greater && equals)
      s += "greater or equals";
    if (less)
      s += "less";
    if (equals)
s += "equals";
    if (greater)
      s += "greater";
    s += " " + version;
    return s;
    return "#Needs to implement!#";
  }

public:
  std::string pkgName;
  std::string version;
  bool less, equals, greater;
}; //class UserTaskItemToINstall;

typedef std::vector<UserTaskItemToInstall> UserTaskItemToInstallVector;
typedef std::list<UserTaskItemToInstall> UserTaskItemToInstallList;

class UserTask 
{
public:
  UserTask() {}
  ~UserTask() {}

public:
  UserTaskItemToInstallVector itemsToInstall;
  StringSet urlsToInstall;
  StringSet namesToRemove;
}; //class UserTask;

#endif //DEPSOLVER_USER_TASK_H;
