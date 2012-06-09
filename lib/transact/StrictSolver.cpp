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
#include"PackageScope.h"

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
  void isValidTask() const;

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
  translateUserTask(task);
  isValidTask();
  logMsg(LOG_DEBUG, "User task translated: %zu to install, %zu to remove, %zu to upgrade", m_userTaskInstall.size(), m_userTaskRemove.size(), m_userTaskUpgrade.size());
}

void StrictSolver::translateUserTask(const UserTask& userTask)
{
  m_userTaskInstall.clear();
  m_userTaskRemove.clear();
  m_userTaskUpgrade.clear();
  for(UserTaskItemToInstallVector::size_type i = 0;i < userTask.itemsToInstall.size();i++)
    {
      //The following line checks the entire package scope including installed packages, so it can return also the installed package if it matches the user request;
      const VarId varId = 0;//FIXME: = processUserTaskItemToInstall(userTask.itemsToInstall[i]);
      assert(varId != BAD_VAR_ID);
      m_userTaskInstall.push_back(varId);
    }
  for(StringSet::const_iterator it = userTask.namesToRemove.begin() ;it != userTask.namesToRemove.end();it++)
    {
      const PackageId pkgId = m_scope.strToPackageId(*it);
      if (pkgId == BAD_PACKAGE_ID)
	{
	  //FIXME:proper user notification;
	  logMsg(LOG_DEBUG, "Request contains ask to remove unknown package \'%s\', skipping", it->c_str());
	  continue;
	}
      VarIdVector vars;
      m_scope.selectInstalledNoProvides(pkgId, vars);
      if (vars.empty())
	{
	  //FIXME:proper user notification;
	  logMsg(LOG_DEBUG, "Request contains ask to remove not installed package \'%s\', skipping", it->c_str());
	  continue;
	}
      if (vars.size() > 1)
	logMsg(LOG_WARNING, "The package \'%s\' is installed %zu times", it->c_str(), vars.size());
      for(VarIdVector::size_type k = 0;k < vars.size();k++)
	m_userTaskRemove.push_back(vars[k]);
    }
  VarIdVector v;
  for(VarIdVector::size_type i = 0;i < m_userTaskInstall.size();i++)
    {
      if (m_scope.isInstalled(m_userTaskInstall[i]))
	{
	  //FIXME:Proper user notification;
	  logMsg(LOG_DEBUG, "Accepted to install variable %zu is already installed, skipping", m_userTaskInstall[i]);
	  continue;
	}
      v.push_back(m_userTaskInstall[i]);
    }
  m_userTaskInstall.clear();
      //FIXME:to upgrade;
  //  removeDublications(m_userTaskInstall);
  //  removeDublications(m_userTaskRemove);
}

void StrictSolver::isValidTask() const
{
  /*FIXME:
  for(VarIdVector::size_type i1 = 0;i1 < strongToInstall.size();i1++)
    for(VarIdVector::size_type i2 = 0;i2 < strongToRemove.size();i2++)
      if (strongToInstall[i1] == strongToRemove[i2])
	{
	  const std::string pkgName = m_scope.packageIdToStr(strongToInstall[i1]);
	  assert(!pkgName.empty());
	  throw TaskException(TaskErrorBothInstallRemove, pkgName);
	}
  */
}
