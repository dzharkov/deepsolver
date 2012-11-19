/*
   Copyright 2011-2012 ALT Linux
   Copyright 2011-2012 Michael Pozhidaev

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

#include"transact/AbstractPackageScope.h"
#include"UserTask.h"

struct TaskSolverData
{
  TaskSolverData(AbstractPackageScope& s)
    : scope(s) {}

  AbstractPackageScope& scope;
}; //struct TaskSolverData;

class AbstractTaskSolver
{
public:
  AbstractTaskSolver() {}
  virtual ~AbstractTaskSolver() {}

public:
  virtual void solve(const UserTask& task, VarIdVector& toInstall, VarIdVector& toRemove) = 0;
  virtual void constructSat() = 0;
}; //class AbstractTaskSolver;

std::auto_ptr<AbstractTaskSolver> createStrictTaskSolver(const PackageScopeContent& content,
							 const ProvideMap& provideMap,
							 const InstalledReferences& requiresReferences,
							 const InstalledReferences& conflictsReferences);

std::auto_ptr<AbstractTaskSolver> createGeneralTaskSolver(const PackageScopeContent& content,
							 const ProvideMap& provideMap,
							 const InstalledReferences& requiresReferences,
							 const InstalledReferences& conflictsReferences);

#endif //DEEPSOLVER_ABSTRACT_TASK_SOLVER_H;
