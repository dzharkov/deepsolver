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
#include"GeneralSolver.h"
#include"PkgUtils.h"
#include"PackageNameSorting.h"

template<typename T>
void rmDub(std::vector<T>& v)
{
  if (v.size() < 64)
    {
      std::vector<T> vv;
      for (size_t i = 0;i < v.size();i++)
	{
	  size_t j;
	  for(j = 0;j < i;j++)
	    if (v[i] == v[j])
	      break;
	  if (j == i)
	  vv.push_back(v[i]);
	}
      v = vv;
      return;
    }
  //Be careful: the following only for items with operator < and operator >;
  std::set<T> s;
  for(size_t i = 0;i < v.size();i++)
    s.insert(v[i]);
  v.resize(s.size());
  size_t k = 0;
  typename std::set<T>::const_iterator it;
  for(it = s.begin();it != s.end();it++)
    v[k++] = *it;
  assert(k == s.size());
}

void GeneralSolver::solve(const UserTask& task, VarIdVector& toInstall, VarIdVector& toRemove)
{
  m_annotating = 0;
  constructSatImpl(task);
  std::auto_ptr<AbstractSatSolver> satSolver = createLibMinisatSolver();
  AbstractSatSolver::VarIdToBoolMap res, assumptions;
  for(Sat::size_type i = 0;i < m_sat.size();i++)
    {
      satSolver->addClause(m_sat[i]);
      //      for(Clause::size_type k = 0;k < m_sat[i].size();k++)
      //	assumptions.insert(AbstractSatSolver::VarIdToBoolMap::value_type(m_sat[i][k].varId, m_scope.isInstalled(m_sat[i][k].varId)));
    }
  logMsg(LOG_DEBUG, "general:launching minisat with %zu clauses", m_sat.size());
  if (!satSolver->solve(assumptions, res))
    throw TaskException(TaskException::NoSatSolution);
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
  logMsg(LOG_INFO, "general:solution found: %zu to install, %zu to remove", toInstall.size(), toRemove.size());
}

std::string GeneralSolver::constructSat(const UserTask& task)
{
  m_annotating = 1;
  constructSatImpl(task);
  return PkgUtils::satToString(m_scope, m_sat, m_annotations);
}

void GeneralSolver::constructSatImpl(const UserTask& task)
{
  const clock_t satConstructStart = clock(); 
  translateUserTask(task);
  processPendings();
  const double satConstructDuration = ((double)clock() - satConstructStart) / CLOCKS_PER_SEC;
    for(Sat::size_type i = 0;i < m_sat.size();i++)
      rmDub(m_sat[i]);
  logMsg(LOG_DEBUG, "general:SAT construction takes %f sec", satConstructDuration);
}

void GeneralSolver::translateUserTask(const UserTask& userTask)
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
      const PackageId pkgId = m_scope.strToPackageId(*it);
      if (pkgId == BAD_PACKAGE_ID)
	{
	  //FIXME:user Notice;
	  logMsg(LOG_DEBUG, "general:request contains ask to remove unknown package \'%s\', skipping", it->c_str());
	  continue;
	}
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

VarId GeneralSolver::translateItemToInstall(const UserTaskItemToInstall& item) 
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

VarId GeneralSolver::satisfyRequire(const IdPkgRel& rel)
{
  if (!rel.hasVer())
    return satisfyRequire(rel.pkgId);
  return satisfyRequire(rel.pkgId, rel.extractVersionCond());
}

VarId GeneralSolver::satisfyRequire(PackageId pkgId)
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

VarId GeneralSolver::satisfyRequire(PackageId pkgId, const VersionCond& version)
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

void GeneralSolver::handleChangeToTrue(VarId varId, bool includeItself)
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
	if (includeItself)
	  clause.push_back(Lit(varId, 1));
	clause.push_back(Lit(otherVersions[i], 1));
	addClause(clause);
	m_pending.push_back(otherVersions[i]);
	if (m_annotating)
	  m_annotations.push_back("# \"" + m_scope.constructPackageNameWithBuildTime(otherVersions[i]) + "\" cannot be installed if \"" + m_scope.constructPackageNameWithBuildTime(varId) + "\" is present in the system");
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
	annotation = "# By the require entry \"" + relToString(requires[i]) + "\" of \"" + m_scope.constructPackageNameWithBuildTime(varId) + "\":";
      Clause clause;
      if (includeItself)
	clause.push_back(Lit(varId, 1));
      VarIdVector installed;
      m_scope.whatSatisfiesAmongInstalled(requires[i], installed);
      rmDub(installed);
      for(VarIdVector::size_type k = 0;k < installed.size();k++)
	{
	  assert(m_scope.isInstalled(installed[k]));
	  if (m_annotating)
	    annotation += "\n# \"" + m_scope.constructPackageNameWithBuildTime(installed[k]) + "\" is already installed";
	  clause.push_back(Lit(installed[k]));
	  m_pending.push_back(installed[k]);
	}
      if (installed.empty())
	{
	  const VarId def = satisfyRequire(requires[i]);
	  assert(def != BAD_VAR_ID);
	  VarIdVector::size_type k;
	  for(k = 0;k < installed.size();k++)
	    if (def == installed[k])
	      break;
	  if (k >= installed.size())
	    {
	      assert(!m_scope.isInstalled(def));
	      if (m_annotating)
		annotation += "\n# \"" + m_scope.constructPackageNameWithBuildTime(def) + "\" is a default matching package among non-installed";
	      clause.push_back(Lit(def));
	      m_pending.push_back(def);
	    }
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
	    if (includeItself)
	      clause.push_back(Lit(varId, 1));
	    clause.push_back(Lit(vars[k], 1));
	    addClause(clause);
	    m_pending.push_back(vars[k]);
	    if (m_annotating)
	      m_annotations.push_back("# \"" + m_scope.constructPackageNameWithBuildTime(vars[k]) + "\" cannot be installed with \"" + m_scope.constructPackageNameWithBuildTime(varId) + "\"");
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
      if (includeItself)
	clause.push_back(Lit(varId, 1));
      clause.push_back(Lit(vars[i], 1));
      addClause(clause);
      m_pending.push_back(vars[i]);
      if (m_annotating)
	m_annotations.push_back("# \"" + m_scope.constructPackageNameWithBuildTime(vars[i]) + "\" currently present in the system should be removed if \"" + m_scope.constructPackageNameWithBuildTime(varId) + "\" is considered for installation");
    }
}

void GeneralSolver::handleChangeToFalse(VarId seed, bool includeItself)
{
  assert(seed != BAD_VAR_ID);
  VarIdVector deps;
  IdPkgRelVector rels;
  m_scope.whatDependsAmongInstalled(seed, deps, rels);
  assert(deps.size() == rels.size());
  for(VarIdVector::size_type i = 0;i < deps.size();i++)
    {
      std::string annotation = "# Installed \"" + m_scope.constructPackageNameWithBuildTime(deps[i]) + "\" depends on installed \"" + m_scope.constructPackageNameWithBuildTime(seed) + "\" by its require \"" + relToString(rels[i]) + "\":";
      Clause clause;
      if (includeItself)
	clause.push_back(Lit(seed));
      clause.push_back(Lit(deps[i], 1));
      m_pending.push_back(deps[i]);
      VarIdVector installed;
      m_scope.whatSatisfiesAmongInstalled(rels[i], installed);
      for(VarIdVector::size_type k = 0;k < installed.size();k++)
	if (installed[k] != seed)
	  {
	    assert(m_scope.isInstalled(installed[k]));
	    clause.push_back(Lit(installed[k]));
	    m_pending.push_back(installed[k]);
	    if (m_annotating)
	      annotation += "\n# " + m_scope.constructPackageNameWithBuildTime(installed[k]) + "\" is installed and also matches this require entry";
	  }
      if (installed.empty())
	{
	  const VarId replacement = satisfyRequire(rels[i]);
	  assert(replacement != BAD_VAR_ID);
	  VarIdVector::size_type k;
	  for(k = 0;k < installed.size();k++)
	    if (replacement == installed[k])
	      break;
	  if (k >= installed.size())
	    {
	      assert(!m_scope.isInstalled(replacement));
	      clause.push_back(Lit(replacement));
	      m_pending.push_back(replacement);
	      if (m_annotating)
		annotation += "\n# \"" + m_scope.constructPackageNameWithBuildTime(replacement) + "\" is considered as default replacement among non-installed";
	    }
	    }
      addClause(clause);
      if (m_annotating)
	m_annotations.push_back(annotation);
    }
}

void GeneralSolver::processPendings()
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
	  if (m_decisionMadeFalse.find(varId) != m_decisionMadeFalse.end())//Installation of varId will never happen;
	    continue;
	  handleChangeToTrue(varId, m_decisionMadeTrue.find(varId) == m_decisionMadeTrue.end());
	} else
	{
	  //Handling the case varId is pending to be removed;
	  if (m_decisionMadeTrue.find(varId) != m_decisionMadeTrue.end())//Removing of varId will never happen;
	    continue;
	  handleChangeToFalse(varId, m_decisionMadeFalse.find(varId) == m_decisionMadeFalse.end());
	}
    }
}

VarId GeneralSolver::processPriorityList(const VarIdVector& vars, PackageId provideEntry)
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

VarId GeneralSolver::processPriorityBySorting(const VarIdVector& vars)
{
  assert(!vars.empty());
  PackageNameSortItemVector items;
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    items.push_back(PackageNameSortItem(&m_backEnd, vars[i], m_scope.constructPackageName(vars[i])));
  std::sort(items.begin(), items.end());
  assert(!items.empty());
  return items[items.size() - 1].varId;
}

void GeneralSolver::addClause(const Clause& clause)
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
	  logMsg(LOG_ERR, "\'%s\' must be simultaneously installed and removed", m_scope.constructPackageNameWithBuildTime(lit.varId).c_str());
	  //FIXME:	  assert(0);
	}
      m_decisionMadeFalse.insert(lit.varId); 
    } else
    {
      if(m_decisionMadeFalse.find(lit.varId) != m_decisionMadeFalse.end())
	{
	  logMsg(LOG_ERR, "\'%s\' must be simultaneously installed and removed", m_scope.constructPackageNameWithBuildTime(lit.varId).c_str());
	  //FIXME:	  assert(0);
	}
    m_decisionMadeTrue.insert(lit.varId);
    }
}

std::string GeneralSolver::relToString(const IdPkgRel& rel)
{
  const std::string ver = rel.verString();
  if (ver.empty())
      return m_scope.packageIdToStr(rel.pkgId);
  return m_scope.packageIdToStr(rel.pkgId) + " " + ver;
}

std::auto_ptr<AbstractTaskSolver> createGeneralTaskSolver(TaskSolverData& taskSolverData)
{
  return std::auto_ptr<AbstractTaskSolver>(new GeneralSolver(taskSolverData));
}
