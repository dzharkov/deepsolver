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

#include"PackageScopeContent.h"
#include"UserTask.h"
#include"TaskException.h"

class AbstractTaskSolver
{
public:
  AbstractTaskSolver() {}
  virtual ~AbstractTaskSolver() {}

public:
  virtual void solve(const PackageScopeContent& content, const UserTask& task, const VarIdVector toInstall, const VarIdVector& toRemove) = 0;
}; //class AbstractTaskSolver;

std::auto_ptr<AbstractTaskSolver> createStrictTaskSolver();

#endif //DEEPSOLVER_ABSTRACT_TASK_SOLVER_H;
