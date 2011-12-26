

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
  PackageScope& m_scope;
}; //class taskPreprocessor;

#endif //DEPSOLVER_TASK_PREPROCESSOR_H;
