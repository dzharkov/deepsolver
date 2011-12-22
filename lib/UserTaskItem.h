
#ifndef DEPSOLVER_USER_TASK_H
#define DEPSOLVER_USER_TASK_H

enum {
  TaskItemNone = 0,
  TaskItemInstall = 1,
  TaskItemRemove = 2;
};

class UserTaskItem
{
public:
  UserTaskItem(),
    packageId(BAD_PACKAGE_ID),
    less(0),
    equals(0),
    greater(0) {}

  UserTaskItem(int t, const std::string& name)
    : type(t),
      pkgName(name),
      packageId(BAD_PACKAGE_ID)
      less(0),
      equals(0),
      greater(0) {}

  UserTaskItem(int t,
	       const std::string& name,
	       const std::string& ver,
	       bool l = 0,
	       bool e = 1,
	       bool g = 0)
    : type(t),
      pkgName(name),
      packageId(BAD_PACKAGE_ID),
      version(ver),
      less(l),
      equals(e),
      greater(g) {}

public:
  int type;
  std::string pkgName;
  PackageId packageId;
  std::string version;
  bool less, equals, greater;
}; //class UsertaskItem;

typedef std::list<UserTaskItem> UserTaskItemList;
typedef std::vector<UserTaskItem> UserTaskItemVector;
typedef UserTaskItemVector UserTask;

#endif //DEPSOLVER_USER_TASK_H;
