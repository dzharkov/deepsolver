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
  StrictSolver(const PackageScopeContent& content) 
    : m_content(content) , m_scope(content) {}

  virtual ~StrictSolver() {}

public:
  void solve(const UserTask& task, VarIdVector& toInstall, VarIdVector& toRemove, VarIdToVarIdMap& toUpgrade);

private:
  void translateUserTask(const UserTask& userTask);

private:
  const PackageScopeContent& m_content;
  PackageScope m_scope; 
  VarIdVector m_userTaskInstall, m_userTaskRemove;
  VarIdToVarIdMap m_userTaskUpgrade;
}; //class StrictSolver;

std::auto_ptr<AbstractTaskSolver> createStrictTaskSolver(const PackageScopeContent& content)
{
  logMsg(LOG_DEBUG, "Creating strict task solver");
  return std::auto_ptr<AbstractTaskSolver>(new StrictSolver(content));
}

void StrictSolver::solve(const UserTask& task, VarIdVector& toInstall, VarIdVector& toRemove, VarIdToVarIdMap& toUpgrade)
{
  translateUserTask(userTask);
  logMsg(LOG_DEBUG, "User task translated: %zu to install, %zu to remove, %zu to upgrade", m_userTaskInstall.size(), m_userTaskRemove.size(), m_userTaskUpgrade.size());
}

void StrictSolver::translateUserTask(const UserTask& userTask)
{
  for(UserTaskItemToInstallVector::size_type i = 0;i < userTask.itemsToInstall.size();i++)
    {
      const VarId varId = processUserTaskItemToInstall(userTask.itemsToInstall[i]);
      assert(varId != BAD_VAR_ID);
      strongToInstall.push_back(varId);
    }
  for(StringSet::const_iterator it = userTask.namesToRemove.begin() ;it != userTask.namesToRemove.end();it++)
    {
      const PackageId pkgId = m_scope.strToPackageId(*it);
      if (pkgId == BAD_PACKAGE_ID)
	throw TaskException(TaskErrorUnknownPackageName, *it);
      VarIdVector vars;
      //The following line does not take into account provide entries;
      m_scope.selectMatchingVars(pkgId, vars);
      for(VarIdVector::size_type k = 0;k < vars.size();k++)
	strongToRemove.push_back(vars[k]);
    }
  for(VarIdVector::size_type i1 = 0;i1 < strongToInstall.size();i1++)
    for(VarIdVector::size_type i2 = 0;i2 < strongToRemove.size();i2++)
      if (strongToInstall[i1] == strongToRemove[i2])
	{
	  const std::string pkgName = m_scope.packageIdToStr(strongToInstall[i1]);
	  assert(!pkgName.empty());
	  throw TaskException(TaskErrorBothInstallRemove, pkgName);
	}
  removeDublications(strongToInstall);
  removeDublications(strongToRemove);
}
