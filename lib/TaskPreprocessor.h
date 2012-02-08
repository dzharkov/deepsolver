

#ifndef DEPSOLVER_TASK_PREPROCESSOR_H
#define DEPSOLVER_TASK_PREPROCESSOR_H

#include"PackageScope.h"
#include"UserTask.h"
#include"ProvidePriorityList.h"

//Must throw the TaskException on any error;
class TaskPreprocessor
{
public:
 TaskPreprocessor(PackageScope& scope, const ProvidePriorityList& providePriorityList)
   : m_scope(scope), m_providePriorityList(providePriorityList) {}

  ~TaskPreprocessor() {}

public:
  void preprocess(const UserTask& userTask,
		  VarIdVector& strongToInstall,
		  VarIdVector& strongToRemove);
  void buildDepClosure(VarId varId, VarIdSet& required, VarIdSet& conflicted);

private:
  void processConflicts(VarId varId, VarIdVector& res);
  void processRequires(VarId varId, VarIdVector& dependent);
  VarId processRequireWithVersion(PackageId pkgId, const VersionCond& cond);
  VarId processRequireWithoutVersion(PackageId pkgId);
  //Never returns BAD_PACKAGE_ID, must throw an exception;
  VarId processUserTaskItemToInstall(const UserTaskItemToInstall& item);
  //Never returns BAD_PACKAGE_ID, must throw an exception;
  VarId processPriority(const VarIdVector& vars, PackageId provideEntry) const;

private:
  PackageScope& m_scope;
  const ProvidePriorityList m_providePriorityList;
}; //class taskPreprocessor;

#endif //DEPSOLVER_TASK_PREPROCESSOR_H;
