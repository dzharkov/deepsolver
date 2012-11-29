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

#ifndef DEEPSOLVER_GENERAL_SOLVER_H
#define DEEPSOLVER_GENERAL_SOLVER_H

#include"AbstractTaskSolver.h"
#include"AbstractPackageBackEnd.h"
#include"AbstractPackageScope.h"
#include"AbstractSatSolver.h"

class GeneralSolver: public AbstractTaskSolver
{
public:
  GeneralSolver(TaskSolverData& data)
    : m_annotating(0),
      m_advancedMode(1),
    m_backEnd(data.backEnd),
      m_scope(data.scope) {}

  virtual ~GeneralSolver() {}

public:
  void solve(const UserTask& task, VarIdVector& toInstall, VarIdVector& toRemove);
  std::string constructSat(const UserTask& task);

private:
  void constructSatImpl(const UserTask& task);
  void handleChangeToFalse(VarId seed,
			   bool includeItself,
			   VarIdVector& involvedInstalled,
			   VarIdVector& involvedRemoved);

  void handleChangeToTrue(VarId varId,
			  bool includeItself,
			  VarIdVector& involvedInstalled,
			  VarIdVector& involvedRemoved);

  void addClause(const Clause& clause);
  void processPendings();
  VarId processPriorityBySorting(const VarIdVector& vars);
  std::string relToString(const IdPkgRel& rel);
  VarId processPriorityList(const VarIdVector& vars, PackageId provideEntry);
  VarId satisfyRequire(const IdPkgRel& rel);
  VarId satisfyRequire(PackageId pkgId);
  VarId satisfyRequire(PackageId pkgId, const VersionCond& version);
  VarId translateItemToInstall(const UserTaskItemToInstall& item);
  void translateUserTask(const UserTask& userTask);

private:
  bool m_annotating, m_advancedMode;
  StringVector m_annotations;
  const AbstractPackageBackEnd& m_backEnd;
  AbstractPackageScope& m_scope; 
  Sat m_sat;
  VarIdVector m_userTaskPresent, m_userTaskAbsent;
  VarIdVector m_pendingInstalled, m_pendingRemoved;
  VarIdSet m_processedInstalled, m_processedRemoved;
  VarIdSet m_decisionMadeTrue, m_decisionMadeFalse;
}; //class GeneralSolver;

#endif //DEEPSOLVER_GENERAL_SOLVER_H;
