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

#ifndef DEEPSOLVER_TASK_PREPROCESSOR_H
#define DEEPSOLVER_TASK_PREPROCESSOR_H

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
  VarId processPriorityList(const VarIdVector& vars, PackageId provideEntry) const;
  //Never returns BAD_PACKAGE_ID, must throw an exception;
  VarId processPriorityBySorting(const VarIdVector& vars) const;

private:
  PackageScope& m_scope;
  const ProvidePriorityList m_providePriorityList;
}; //class taskPreprocessor;

#endif //DEEPSOLVER_TASK_PREPROCESSOR_H;
