

#ifndef DEPSOLVER_TASK_PREPROCESSOR_H
#define DEPSOLVER_TASK_PREPROCESSOR_H

#include"PackageScope.h"
#include"UserTask.h"

class TaskPreprocessor
{
public:
  TaskPreprocessor(PackageScope& scope)
    : m_scope(scope) {}

  ~TaskPreprocessor() {}

public:
  void preprocess(const UserTask& userTask);

private:
  //Never returns BAD_PACKAGE_ID, must throw an exception;
  VarId processUserTaskItemToInstall(const UserTaskItemToInstall& item);
  //Never returns BAD_PACKAGE_ID, must throw an exception;
  VarId processPriority(const VarIdVector& vars, PackageId provideEntry) const;

private:
  PackageScope& m_scope;
}; //class taskPreprocessor;

#endif //DEPSOLVER_TASK_PREPROCESSOR_H;
