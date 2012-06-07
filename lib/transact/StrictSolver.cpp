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

#include"deepsolver.h"
#include"AbstractTaskSolver.h"

class StrictSolver: public AbstractTaskSolver
{
public:
  StrictSolver()
  virtual ~StrictSolver() {}

public:
  void solve(const PackageScopeContent& content, const UserTask& task, const VarIdVector toInstall, const VarIdVector& toRemove)
  {
    //FIXME:
  }
}; //class StrictSolver;

std::auto_ptr<AbstractTaskSolver> createStrictTaskSolver()
{
  logMsg(LOG_DEBUG, "Creating strict task solver");
  return std::auto_ptr<AbstractTaskSolver>(new StrictSolver());
}
