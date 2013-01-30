/*
   Copyright 2011-2013 ALT Linux
   Copyright 2011-2013 Michael Pozhidaev

   This file is part of the Deepsolver.

   Deepsolver is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   Deepsolver is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
*/

#ifndef DEEPSOLVER_ABSTRACT_TASK_SOLVER_H
#define DEEPSOLVER_ABSTRACT_TASK_SOLVER_H

#include"AbstractPackageScope.h"
#include"AbstractPackageBackEnd.h"

struct TaskSolverProvideInfo
{
  TaskSolverProvideInfo() {}

  TaskSolverProvideInfo(const std::string& n)
    : name(n) {}

  std::string name;
  StringVector providers;
}; //struct TaskSolverProvideInfo;

typedef std::vector<TaskSolverProvideInfo> TaskSolverProvideInfoVector;
typedef std::list<TaskSolverProvideInfo> TaskSolverProvideInfoList;

struct TaskSolverData
{
  TaskSolverData(const AbstractPackageBackEnd& b, AbstractPackageScope& s)
    : backEnd(b),
      scope(s) {}

  const AbstractPackageBackEnd& backEnd;
  AbstractPackageScope& scope;
  TaskSolverProvideInfoVector provides;
}; //struct TaskSolverData;

class AbstractTaskSolver
{
public:
  AbstractTaskSolver() {}
  virtual ~AbstractTaskSolver() {}

public:
  virtual void solve(const UserTask& task, VarIdVector& toInstall, VarIdVector& toRemove) = 0;
  virtual std::string constructSat(const UserTask& task) = 0;
}; //class AbstractTaskSolver;

std::auto_ptr<AbstractTaskSolver> createGeneralTaskSolver(TaskSolverData& taskSolverData);
std::auto_ptr<AbstractTaskSolver> createRelaxedTaskSolver(TaskSolverData& taskSolverData);

#endif //DEEPSOLVER_ABSTRACT_TASK_SOLVER_H;
