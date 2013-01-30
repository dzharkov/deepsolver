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

#include"deepsolver.h"
#include"RelaxedSolver.h"
#include"PkgUtils.h"
#include"PackageNameSorting.h"

void RelaxedSolver::solve(const UserTask& task, VarIdVector& toInstall, VarIdVector& toRemove)
{
  m_annotating = 0;
  constructSatImpl(task);
  if (m_sat.empty())
    return;
  std::auto_ptr<AbstractSatSolver> satSolver = createLibMinisatSolver();
  AbstractSatSolver::VarIdToBoolMap res, assumptions;
  for(Sat::size_type i = 0;i < m_sat.size();i++)
    {
      satSolver->addClause(m_sat[i]);
      //      for(Clause::size_type k = 0;k < m_sat[i].size();k++)
      //	assumptions.insert(AbstractSatSolver::VarIdToBoolMap::value_type(m_sat[i][k].varId, m_scope.isInstalled(m_sat[i][k].varId)));
    }
  VarIdVector solutionConflicts;
  logMsg(LOG_DEBUG, "relaxed:launching minisat with %zu clauses", m_sat.size());
  if (!satSolver->solve(assumptions, res, solutionConflicts))
    {
      logMsg(LOG_DEBUG, "relaxed:minisat fails with %zu conflicts:", solutionConflicts.size());
      for(VarIdVector::size_type i = 0;i < solutionConflicts.size();i++)
	logMsg(LOG_DEBUG, "relaxed:%s", m_scope.constructPackageName(solutionConflicts[i]).c_str());
      throw TaskException(TaskException::NoSatSolution);
    }
  for(AbstractSatSolver::VarIdToBoolMap::const_iterator it = res.begin();it != res.end();it++)
    if (it->second)
      {
	if (!m_scope.isInstalled(it->first))
	  toInstall.push_back(it->first); 
      }else
      {
	if (m_scope.isInstalled(it->first))
	  toRemove.push_back(it->first);
      }
  logMsg(LOG_INFO, "relaxed:solution found: %zu to install, %zu to remove", toInstall.size(), toRemove.size());
}

std::string RelaxedSolver::constructSat(const UserTask& task)
{
  m_annotating = 1;
  constructSatImpl(task);
  return PkgUtils::satToString(m_scope, m_sat, m_annotations);
}

void RelaxedSolver::constructSatImpl(const UserTask& task)
{
  const clock_t satConstructStart = clock(); 
  translateUserTask(task);
  processPendings();
  const double satConstructDuration = ((double)clock() - satConstructStart) / CLOCKS_PER_SEC;
    for(Sat::size_type i = 0;i < m_sat.size();i++)
      rmDub(m_sat[i]);
  logMsg(LOG_DEBUG, "relaxed:SAT construction takes %f sec", satConstructDuration);
}

void RelaxedSolver::translateUserTask(const UserTask& userTask)
{
  m_sat.clear();
  m_annotations.clear();
  m_pending.clear();
  m_processed.clear();
  m_decisionMadeFalse.clear();
  m_decisionMadeTrue.clear();

  // To install;
  for(UserTaskItemToInstallVector::size_type i = 0;i < userTask.itemsToInstall.size();i++)
    {
      //The following line checks the entire package scope including installed packages, so it can return also the installed package if it matches the user request;
      const VarId varId = translateItemToInstall(userTask.itemsToInstall[i]);
      assert(varId != BAD_VAR_ID);
      m_pending.push_back(varId);
      addClause(unitClause(Lit(varId)));
      if (m_annotating)
	m_annotations.push_back("# Buy user task to install \"" + userTask.itemsToInstall[i].toString() + "\"");
      VarIdVector otherVersions;
      m_scope.selectMatchingVarsRealNames(m_scope.packageIdOfVarId(varId), otherVersions);
      rmDub(otherVersions);
      for(VarIdVector::size_type k = 0;k < otherVersions.size();k++)
	if (otherVersions[k] != varId)
	  {
	    m_pending.push_back(otherVersions[k]);//FIXME:Not every package require this;
	    addClause(unitClause(Lit(otherVersions[k], 1)));
	    if (m_annotating)
	      m_annotations.push_back("# Blocked by user task to install \"" + userTask.itemsToInstall[i].toString() + "\"");
	  }
    } //For items to install;

  // To remove;
  for(StringSet::const_iterator it = userTask.namesToRemove.begin() ;it != userTask.namesToRemove.end();it++)
    {
      assert(!it->empty());
      if (!m_scope.checkName(*it))
	{
	  //FIXME:user Notice;
	  logMsg(LOG_DEBUG, "relaxed:request contains ask to remove unknown package \'%s\', skipping", it->c_str());
	  continue;
	}
      const PackageId pkgId = m_scope.strToPackageId(*it);
      assert(pkgId != BAD_PACKAGE_ID);
      VarIdVector vars;
      m_scope.selectMatchingVarsRealNames(pkgId, vars);
      rmDub(vars);
      for(VarIdVector::size_type k = 0;k < vars.size();k++)
	{
	  m_pending.push_back(vars[k]);
	  addClause(unitClause(Lit(vars[k], 1)));
	  if (m_annotating)
	    m_annotations.push_back("# By user task to remove \"" + *it + "\"");
	}
    }
}

VarId RelaxedSolver::translateItemToInstall(const UserTaskItemToInstall& item) 
{
  //FIXME:Obsoletes;
  assert(!item.pkgName.empty());
  const bool hasVersion = item.verDir != VerNone;
  assert(!hasVersion || !item.version.empty());
  if (!m_scope.checkName(item.pkgName))
    throw TaskException(TaskException::UnknownPackageName, item.pkgName);
  const PackageId pkgId = m_scope.strToPackageId(item.pkgName);
  assert(pkgId != BAD_PACKAGE_ID);
  VarIdVector vars;
  if (!hasVersion)
    {
      //The following line does not take into account available provides;
      m_scope.selectMatchingVarsRealNames(pkgId, vars);
    } else
    {
      VersionCond ver(item.version, item.verDir);
      //This line does not handle provides too;
      m_scope.selectMatchingVarsRealNames(pkgId, ver, vars);
    }
  if (!vars.empty())
    {
      m_scope.selectTheNewest(vars);
      assert(!vars.empty());
      //We can get here more than one the newest packages, assuming no difference what exact one to take;
      return vars.front();
    }
  /*
   * We cannot find anything just by real names (no package with required
   * name at all or there have inappropriate version restrictions ), so 
   * now the time to select anything among presented provides records;
   */
  if (hasVersion)
    {
      VersionCond ver(item.version, item.verDir);
      m_scope.selectMatchingVarsAmongProvides(pkgId, ver, vars);
    } else
    m_scope.selectMatchingVarsAmongProvides(pkgId, vars);
  if (vars.empty())//No appropriate packages at all;
    throw TaskException(TaskException::NoRequestedPackage, item.toString());
  if (hasVersion || m_scope.allProvidesHaveTheVersion(vars, pkgId))
    {
      const VarId res = processPriorityList(vars, pkgId);
      if (res != BAD_VAR_ID)
	return res;
      m_scope.selectTheNewestByProvide(vars, pkgId);
      assert(!vars.empty());
      if (vars.size() == 1)
	return vars.front();
      return processPriorityBySorting(vars);
    }
  const VarId res = processPriorityList(vars, pkgId);
  if (res != BAD_VAR_ID)
    return res;
  return processPriorityBySorting(vars);
}

VarId RelaxedSolver::satisfyRequire(const IdPkgRel& rel)
{
  if (!rel.hasVer())
    return satisfyRequire(rel.pkgId);
  return satisfyRequire(rel.pkgId, rel.extractVersionCond());
}

VarId RelaxedSolver::satisfyRequire(PackageId pkgId)
{
  assert(pkgId != BAD_PACKAGE_ID);
  VarIdVector vars;
  //The following line does not take into account available provides;
  m_scope.selectMatchingVarsRealNames(pkgId, vars);
  if (!vars.empty())
    {
      m_scope.selectTheNewest(vars);
      assert(!vars.empty());
      //We can get here more than one the newest packages, assuming no difference what exact one to take;
      return vars.front();
    }
  /*
   * We cannot find anything just by real names, so 
   * now the time to select anything among presented provides records;
   */
  //  logMsg(LOG_DEBUG, "Need to search by provides for \'%s\'", m_scope.packageIdToStr(pkgId).c_str());
  m_scope.selectMatchingVarsAmongProvides(pkgId, vars);
  if (vars.empty())//No appropriate packages at all;
    {
      logMsg(LOG_ERR, "No matching package for require entry \'%s\'", m_scope.packageIdToStr(pkgId).c_str());
      throw TaskException(TaskException::Unmet, m_scope.packageIdToStr(pkgId));
    }
  if (m_scope.allProvidesHaveTheVersion(vars, pkgId))
    {
      const VarId res = processPriorityList(vars, pkgId);
      if (res != BAD_VAR_ID)
	return res;
      m_scope.selectTheNewestByProvide(vars, pkgId);
      assert(!vars.empty());
      if (vars.size() == 1)
	return vars.front();
      return processPriorityBySorting(vars);
    }
  const VarId res = processPriorityList(vars, pkgId);
  if (res != BAD_VAR_ID)
    return res;
  return processPriorityBySorting(vars);
}

VarId RelaxedSolver::satisfyRequire(PackageId pkgId, const VersionCond& version)
{
  assert(version.type != VerNone);
  assert(!version.version.empty());
  assert(pkgId != BAD_PACKAGE_ID);
  VarIdVector vars;
  //This line does not handle provides ;
  m_scope.selectMatchingVarsRealNames(pkgId, version, vars);
  if (!vars.empty())
    {
      m_scope.selectTheNewest(vars);
      assert(!vars.empty());
      //We can get here more than one the newest packages, assuming no difference what exact one to take;
      return vars.front();
    }
  /*
   * We cannot find anything just by real names, so 
   * now the time to select anything among presented provides records;
   */
  m_scope.selectMatchingVarsAmongProvides(pkgId, version, vars);
  if (vars.empty())//No appropriate packages at all;
    {
      logMsg(LOG_ERR, "No matching package for require entry \'%s\'", relToString(IdPkgRel(pkgId, version)).c_str());
      throw TaskException(TaskException::Unmet, m_scope.packageIdToStr(pkgId));
    }
  const VarId res = processPriorityList(vars, pkgId);
  if (res != BAD_VAR_ID)
    return res;
  m_scope.selectTheNewestByProvide(vars, pkgId);
  assert(!vars.empty());
  if (vars.size() == 1)
    return vars.front();
  return processPriorityBySorting(vars);
}

void RelaxedSolver::handleChangeToTrue(VarId varId)
{
  assert(varId != BAD_VAR_ID);
  assert(!m_scope.isInstalled(varId));

  //Blocking other versions of this package;
  VarIdVector otherVersions;
  m_scope.selectMatchingVarsRealNames(m_scope.packageIdOfVarId(varId), otherVersions);
  for(VarIdVector::size_type i = 0;i < otherVersions.size();i++)
    if (otherVersions[i] != varId)
      {
	//FIXME:Not every package requires this;
	Clause clause;
	clause.push_back(Lit(varId, 1));
	clause.push_back(Lit(otherVersions[i], 1));
	addClause(clause);
	m_pending.push_back(otherVersions[i]);
	if (m_annotating)
	  m_annotations.push_back("# \"" + m_scope.constructPackageName(otherVersions[i]) + "\" cannot be installed if \"" + m_scope.constructPackageName(varId) + "\" is present in the system");
      }

  //Requires;
  IdPkgRelVector requires;
  m_scope.getRequires(varId, requires);
  for(IdPkgRelVector::size_type i = 0;i < requires.size();i++)
    {
      IdPkgRelVector::size_type dub;
      for(dub = 0;dub < i;dub++)
	if (requires[dub] == requires[i])
	  break;
      if (dub < i)
	continue;
      std::string annotation;
      if (m_annotating)
	annotation = "# By the require entry \"" + relToString(requires[i]) + "\" of \"" + m_scope.constructPackageName(varId) + "\":";
      Clause clause;
      clause.push_back(Lit(varId, 1));
      VarIdVector alternatives;
      m_scope.selectMatchingVarsWithProvides(requires[i], alternatives);
      rmDub(alternatives);
      for(VarIdVector::size_type k = 0;k < alternatives.size();k++)
	{
	  if (m_annotating)
	    annotation += "\n# \"" + m_scope.constructPackageName(alternatives[k]) + "\" matches";
	  clause.push_back(Lit(alternatives[k]));
	  m_pending.push_back(alternatives[k]);
	}
      addClause(clause);
      if (m_annotating)
	m_annotations.push_back(annotation);
    }

  //Conflicts;
  IdPkgRelVector conflicts;
  m_scope.getConflicts(varId, conflicts);
  for(IdPkgRelVector::size_type i = 0;i < conflicts.size();i++)
    {
      IdPkgRelVector::size_type dub;
      for(dub = 0;dub < i;dub++)
	if (conflicts[dub] == conflicts[i])
	  break;
      if (dub < i)
	continue;
      VarIdVector vars;
      m_scope.selectMatchingVarsWithProvides(conflicts[i], vars);
      for(VarIdVector::size_type k = 0;k < vars.size();k++)
	if (vars[k] != varId)//Package cannot conflict with itself;
	  {
	    Clause clause;
	    clause.push_back(Lit(varId, 1));
	    clause.push_back(Lit(vars[k], 1));
	    addClause(clause);
	    m_pending.push_back(vars[k]);
	    if (m_annotating)
	      m_annotations.push_back("# \"" + m_scope.constructPackageName(vars[k]) + "\" cannot be installed with \"" + m_scope.constructPackageName(varId) + "\"");
	  }
    }

  //Here we check are there any conflicts from the installed packages;
  VarIdVector vars;
  IdPkgRelVector rels;
  m_scope.whatConflictsAmongInstalled(varId, vars, rels);
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    {
      assert(m_scope.isInstalled(vars[i]));
      Clause clause;
      clause.push_back(Lit(varId, 1));
      clause.push_back(Lit(vars[i], 1));
      addClause(clause);
      m_pending.push_back(vars[i]);
      if (m_annotating)
	m_annotations.push_back("# \"" + m_scope.constructPackageName(vars[i]) + "\" currently present in the system should be removed if \"" + m_scope.constructPackageName(varId) + "\" is considered for installation");
    }
}

void RelaxedSolver::handleChangeToFalse(VarId seed)
{
  assert(seed != BAD_VAR_ID);
  VarIdVector deps;
  IdPkgRelVector rels;
  m_scope.whatDependsAmongInstalled(seed, deps, rels);
  assert(deps.size() == rels.size());
  for(VarIdVector::size_type i = 0;i < deps.size();i++)
    {
      std::string annotation = "# Installed \"" + m_scope.constructPackageName(deps[i]) + "\" depends on installed \"" + m_scope.constructPackageName(seed) + "\" by its require \"" + relToString(rels[i]) + "\":";
      Clause clause;
      clause.push_back(Lit(seed));
      clause.push_back(Lit(deps[i], 1));
      m_pending.push_back(deps[i]);
      addClause(clause);
      if (m_annotating)
	m_annotations.push_back(annotation);

      const PackageId pkgId = m_scope.packageIdOfVarId(deps[i]);
      VarIdVector otherVersions;
      m_scope.selectMatchingVarsRealNames(pkgId, otherVersions);
      rmDub(otherVersions);
      for(VarIdVector::size_type k = 0;k < otherVersions.size();k++)
	m_pending.push_back(otherVersions[k]);
    }
}

void RelaxedSolver::processPendings()
{
  while(!m_pending.empty())
    {
      const VarId varId = m_pending.back();
      m_pending.pop_back();
      if (m_processed.find(varId) != m_processed.end())
	continue;
      m_processed.insert(varId);

      if (!m_scope.isInstalled(varId))
	{
	  //Handling the case varId is pending to be installed;
	  handleChangeToTrue(varId);
	} else
	{
	  //Handling the case varId is pending to be removed;
	  handleChangeToFalse(varId);
	}
    }
}

VarId RelaxedSolver::processPriorityList(const VarIdVector& vars, PackageId provideEntry)
{
  assert(!vars.empty());
  const std::string provideName = m_scope.packageIdToStr(provideEntry);
  TaskSolverProvideInfoVector::size_type k = 0;
  while(k < m_taskSolverData.provides.size() && provideName != m_taskSolverData.provides[k].name)
    k++;
  if (k >= m_taskSolverData.provides.size())
    return BAD_VAR_ID;
  const StringVector& providers = m_taskSolverData.provides[k].providers;
  for(StringVector::size_type i = 0;i < providers.size();i++)
    {
      if (!m_scope.checkName(providers[i]))
	continue;
      const PackageId providerId = m_scope.strToPackageId(providers[i]);
      assert(providerId != BAD_PACKAGE_ID);
      for(VarIdVector::size_type k = 0;k < vars.size();k++)
	if (providerId == m_scope.packageIdOfVarId(vars[k]))
	  return vars[k];
    }
  return BAD_VAR_ID;
}

VarId RelaxedSolver::processPriorityBySorting(const VarIdVector& vars)
{
  assert(!vars.empty());
  PackageNameSortItemVector items;
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    items.push_back(PackageNameSortItem(&m_backEnd, vars[i], m_scope.constructPackageName(vars[i])));
  std::sort(items.begin(), items.end());
  assert(!items.empty());
  return items[items.size() - 1].varId;
}

void RelaxedSolver::addClause(const Clause& clause)
{
  assert(!clause.empty());
  m_sat.push_back(clause);
  if (clause.size() != 1)
    return;
  const Lit& lit = clause[0];
  if (lit.neg)
    {
      if(m_decisionMadeTrue.find(lit.varId) != m_decisionMadeTrue.end())
	{
	  logMsg(LOG_ERR, "\'%s\' must be simultaneously installed and removed", m_scope.constructPackageName(lit.varId).c_str());
	  //FIXME:	  assert(0);
	}
      m_decisionMadeFalse.insert(lit.varId); 
    } else
    {
      if(m_decisionMadeFalse.find(lit.varId) != m_decisionMadeFalse.end())
	{
	  logMsg(LOG_ERR, "\'%s\' must be simultaneously installed and removed", m_scope.constructPackageName(lit.varId).c_str());
	  //FIXME:	  assert(0);
	}
    m_decisionMadeTrue.insert(lit.varId);
    }
}

std::string RelaxedSolver::relToString(const IdPkgRel& rel)
{
  const std::string ver = rel.verString();
  if (ver.empty())
      return m_scope.packageIdToStr(rel.pkgId);
  return m_scope.packageIdToStr(rel.pkgId) + " " + ver;
}

std::auto_ptr<AbstractTaskSolver> createRelaxedTaskSolver(TaskSolverData& taskSolverData)
{
  return std::auto_ptr<AbstractTaskSolver>(new RelaxedSolver(taskSolverData));
}
