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
#include"AbstractSatSolver.h"
#include"PackageScope.h"
#include"version.h"

#define NAME_S(x) m_scope.constructPackageName(x)

static void printSat(const PackageScope& scope,
		     const Sat& sat,
		     const StringVector& annotations);
static void printSolution(const PackageScope& scope,
			  const VarIdVector& install,
			  const VarIdVector& remove);

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

struct PrioritySortItem
{
  PrioritySortItem(VarId v, const std::string& n)
    : varId(v), name(n) {}

  bool operator <(const PrioritySortItem& item) const
  {
    return versionCompare(name, item.name) < 0;
  }

  bool operator >(const PrioritySortItem& item) const
  {
    return versionCompare(name, item.name) > 0;
  }

  VarId varId;
  std::string name;
}; //struct PrioritySortItem;

typedef std::vector<PrioritySortItem> PrioritySortItemVector;
typedef std::list<PrioritySortItem> PrioritySortItemList;

class GeneralSolver: public AbstractTaskSolver
{
public:
  GeneralSolver(const PackageScopeContent& content,
	       const ProvideMap& provideMap,
	       const InstalledReferences& requiresReferences,
	       const InstalledReferences& conflictsReferences)
    : m_annotating(1),
      m_content(content) , m_scope(content, provideMap, requiresReferences, conflictsReferences) {}

  virtual ~GeneralSolver() {}

public:
  void solve(const UserTask& task, VarIdVector& toInstall, VarIdVector& toRemove, VarIdToVarIdMap& toUpgrade);

private:
  void translateUserTask(const UserTask& userTask);

  //May not return BAD_VAR_ID;
  VarId translateItemToInstall(const UserTaskItemToInstall& item);

  //May not return BAD_VAR_ID;
  VarId satisfyRequire(const IdPkgRel& rel);

  //May not return BAD_VAR_ID;
  VarId satisfyRequire(PackageId pkgId);

  //May not return BAD_VAR_ID;
  VarId satisfyRequire(PackageId pkgId, const VersionCond& version);
  VarId processPriorityList(const VarIdVector& vars, PackageId provideEntry) const;
  VarId processPriorityBySorting(const VarIdVector& vars) const;

  void handleChangeToFalse(VarId seed,
			      VarIdVector& involvedInstalled,
			      VarIdVector& involvedRemoved);

  void handleChangeToTrue(VarId varId,
				VarIdVector& involvedInstalled,
				VarIdVector& involvedRemoved);

  void processPendings();

private://Utilities;
  std::string relToString(const IdPkgRel& rel);

private:
  bool m_annotating;
  StringVector m_annotations;
  const PackageScopeContent& m_content;
  PackageScope m_scope; 
  Sat m_sat;
  VarIdVector m_userTaskPresent, m_userTaskAbsent;
  VarIdVector m_pendingInstalled, m_pendingRemoved;
  VarIdSet m_processedInstalled, m_processedRemoved;

  VarIdToVarIdMap m_userTaskUpgrade, m_anywayUpgrade;

}; //class GeneralSolver;

std::auto_ptr<AbstractTaskSolver> createGeneralTaskSolver(const PackageScopeContent& content,
							 const ProvideMap& provideMap,
							 const InstalledReferences& requiresReferences,
							 const InstalledReferences& conflictsReferences)
{
  logMsg(LOG_DEBUG, "Creating general task solver");
  return std::auto_ptr<AbstractTaskSolver>(new GeneralSolver(content, provideMap, requiresReferences, conflictsReferences));
}

void GeneralSolver::solve(const UserTask& task, VarIdVector& toInstall, VarIdVector& toRemove, VarIdToVarIdMap& toUpgrade)
{
  const clock_t satConstructStart = clock(); 
  translateUserTask(task);
  logMsg(LOG_DEBUG, "User task translated: %zu to be present, %zu to be absent", m_userTaskPresent.size(), m_userTaskAbsent.size());
  for(VarIdVector::size_type i = 0;i < m_userTaskAbsent.size();i++)
    if (m_scope.isInstalled(m_userTaskAbsent[i]))
      handleChangeToFalse(m_userTaskAbsent[i], m_pendingInstalled, m_pendingRemoved);
  for(VarIdVector::size_type i = 0;i < m_userTaskPresent.size();i++)
    if (!m_scope.isInstalled(m_userTaskPresent[i]))
      handleChangeToTrue(m_userTaskPresent[i], m_pendingInstalled, m_pendingRemoved);
  processPendings();
  const double satConstructDuration = ((double)clock() - satConstructStart) / CLOCKS_PER_SEC;
  logMsg(LOG_DEBUG, "SAT construction takes %f sec", satConstructDuration);

    for(Sat::size_type i = 0;i < m_sat.size();i++)
      rmDub(m_sat[i]);
  printSat(m_scope, m_sat, m_annotations);
  logMsg(LOG_DEBUG, "Creating libminisat SAT solver");
  std::auto_ptr<AbstractSatSolver> satSolver = createLibMinisatSolver();
  for(Sat::size_type i = 0;i < m_sat.size();i++)
    satSolver->addClause(m_sat[i]);
  AbstractSatSolver::VarIdToBoolMap res;
    logMsg(LOG_DEBUG, "Launching minisat with %zu clauses", m_sat.size());
    satSolver->solve(res);
  VarIdVector resInstall, resRemove;
  for(AbstractSatSolver::VarIdToBoolMap::const_iterator it = res.begin();it != res.end();it++)
    if (it->second)
      {
	if (m_scope.isInstalled(it->first))
	  logMsg(LOG_WARNING, "\'%s\' is installed and selected for installation", m_scope.constructPackageNameWithBuildTime(it->first).c_str());
	resInstall.push_back(it->first); 
      }else
      {
	if (!m_scope.isInstalled(it->first))
	  logMsg(LOG_WARNING, "\'%s\' is not installed and selected for removing", m_scope.constructPackageNameWithBuildTime(it->first).c_str());
	resRemove.push_back(it->first);
      }
  printSolution(m_scope, resInstall, resRemove);
}

void GeneralSolver::translateUserTask(const UserTask& userTask)
{
  m_userTaskPresent.clear();
  m_userTaskAbsent.clear();

  // To install;
  for(UserTaskItemToInstallVector::size_type i = 0;i < userTask.itemsToInstall.size();i++)
    {
      //The following line checks the entire package scope including installed packages, so it can return also the installed package if it matches the user request;
      const VarId varId = translateItemToInstall(userTask.itemsToInstall[i]);
      assert(varId != BAD_VAR_ID);
      m_processedInstalled.insert(varId);
      m_userTaskPresent.push_back(varId);
      m_sat.push_back(unitClause(Lit(varId)));
      if (m_annotating)
	m_annotations.push_back("# Buy user task to install \"" + userTask.itemsToInstall[i].toString() + "\"");
      VarIdVector otherVersions;
      m_scope.selectMatchingVarsNoProvides(m_scope.packageIdOfVarId(varId), otherVersions);
      rmDub(otherVersions);
      for(VarIdVector::size_type k = 0;k < otherVersions.size();k++)
	if (otherVersions[k] != varId)
	  {
	    m_userTaskAbsent.push_back(otherVersions[k]);//FIXME:Not every package require this;
	    m_processedRemoved.insert(otherVersions[k]);
	    m_sat.push_back(unitClause(Lit(otherVersions[k], 1)));
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
	  //FIXME:Notice;
	  logMsg(LOG_DEBUG, "Request contains ask to remove unknown package \'%s\', skipping", it->c_str());
	  continue;
	}
      VarIdVector vars;
      m_scope.selectMatchingVarsNoProvides(pkgId, vars);
      rmDub(vars);
      for(VarIdVector::size_type k = 0;k < vars.size();k++)
	{
	  m_userTaskAbsent.push_back(vars[k]);
	  m_processedRemoved.insert(vars[k]);
	  m_sat.push_back(unitClause(Lit(vars[k], 1)));
	  if (m_annotating)
	    m_annotations.push_back("# By user task to remove \"" + *it + "\"");
	}
    }
  rmDub(m_userTaskPresent);
  rmDub(m_userTaskAbsent);
}

VarId GeneralSolver::translateItemToInstall(const UserTaskItemToInstall& item) 
{
  //FIXME:Obsoletes;
  assert(!item.pkgName.empty());
  const bool hasVersion = item.verDir != VerNone;
  assert(!hasVersion || !item.version.empty());
  if (!m_scope.checkName(item.pkgName))
    throw TaskException(TaskErrorUnknownPackageName, item.pkgName);
  const PackageId pkgId = m_scope.strToPackageId(item.pkgName);
  assert(pkgId != BAD_PACKAGE_ID);
  VarIdVector vars;
  if (!hasVersion)
    {
      //The following line does not take into account available provides;
      m_scope.selectMatchingVarsNoProvides(pkgId, vars);
    } else
    {
      VersionCond ver(item.version, item.verDir);
      //This line does not handle provides too;
      m_scope.selectMatchingVarsNoProvides(pkgId, ver, vars);
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
    throw TaskException(TaskErrorNoInstallationCandidat, item.toString());
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
  m_scope.selectMatchingVarsNoProvides(pkgId, vars);
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
  m_scope.selectMatchingVarsAmongProvides(pkgId, vars);
  if (vars.empty())//No appropriate packages at all;
    {
      logMsg(LOG_ERR, "No matching package for require entry \'%s\'", m_scope.packageIdToStr(pkgId).c_str());
      throw TaskException(TaskErrorNoInstallationCandidat, m_scope.packageIdToStr(pkgId));
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
  m_scope.selectMatchingVarsNoProvides(pkgId, version, vars);
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
    throw TaskException(TaskErrorNoInstallationCandidat, m_scope.packageIdToStr(pkgId));
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

void GeneralSolver::handleChangeToTrue(VarId varId,
					 VarIdVector& involvedInstalled,
					 VarIdVector& involvedRemoved)
{
  assert(varId != BAD_VAR_ID);
  assert(!m_scope.isInstalled(varId));
  //  logMsg(LOG_DEBUG, "Processing possibility to be installed for \'%s\'", m_scope.constructPackageName(varId).c_str());

  //Blocking other versions of this package;
  VarIdVector otherVersions;
  m_scope.selectMatchingVarsNoProvides(m_scope.packageIdOfVarId(varId), otherVersions);
  for(VarIdVector::size_type i = 0;i < otherVersions.size();i++)
    if (otherVersions[i] != varId)
      {
	//FIXME:Not every package require this;
	Clause clause;
	clause.push_back(Lit(varId, 1));
	clause.push_back(Lit(otherVersions[i], 1));
	m_sat.push_back(clause);
	involvedRemoved.push_back(otherVersions[i]);
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
      clause.push_back(Lit(varId, 1));
      VarIdVector installed;
      m_scope.whatSatisfiesAmongInstalled(requires[i], installed);
      rmDub(installed);
      //      logMsg(LOG_DEBUG, "%zu packages satisfy among installed", installed.size());
      for(VarIdVector::size_type k = 0;k < installed.size();k++)
	{
	  assert(m_scope.isInstalled(installed[k]));
	  //	  logMsg(LOG_DEBUG, "\'%s\' satisfies and installed", m_scope.constructPackageName(installed[k]).c_str());
	  if (m_annotating)
	    annotation += "\n# \"" + m_scope.constructPackageNameWithBuildTime(installed[k]) + "\" is already installed";
	  clause.push_back(Lit(installed[k]));
	  involvedRemoved.push_back(installed[k]);
	}
      const VarId def = satisfyRequire(requires[i]);
      assert(def != BAD_VAR_ID);
      //      logMsg(LOG_DEBUG, "Found default require solution \'%s\'", m_scope.constructPackageName(def).c_str());
      VarIdVector::size_type k;
      for(k = 0;k < installed.size();k++)
	if (def == installed[k])
	  break;
      if (k >= installed.size())
	{
	  assert(!m_scope.isInstalled(def));
	  //	  logMsg(LOG_DEBUG, "Default require solution \'%s\' is not installed, using it", m_scope.constructPackageName(def).c_str());
	  if (m_annotating)
	    annotation += "\n# \"" + m_scope.constructPackageNameWithBuildTime(def) + "\" is a default matching package among non-installed";
	  clause.push_back(Lit(def));
	  involvedInstalled.push_back(def);
	} else
	//	logMsg(LOG_DEBUG, "Default require solution  \'%s\' is already installed, ignoring it", m_scope.constructPackageName(def).c_str());
      assert(clause.size() >= 2);
      m_sat.push_back(clause);
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
	    m_sat.push_back(clause);
	    involvedRemoved.push_back(vars[k]);
	    //FIXME:If vars[k] is never involved elsewhere it should be explicitly blocked for installation;
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
      clause.push_back(Lit(varId, 1));
      clause.push_back(Lit(vars[i], 1));
      m_sat.push_back(clause);
      involvedRemoved.push_back(vars[i]);
      if (m_annotating)
	m_annotations.push_back("# \"" + m_scope.constructPackageNameWithBuildTime(vars[i]) + "\" currently present in the system should be removed if \"" + m_scope.constructPackageNameWithBuildTime(varId) + "\" is considered for installation");
    }
}

void GeneralSolver::handleChangeToFalse(VarId seed,
					   VarIdVector& involvedInstalled,
					   VarIdVector& involvedRemoved)
{
  assert(m_scope.isInstalled(seed));
  //  logMsg(LOG_DEBUG, "Processing dependence breaks for \'%s\' removing", m_scope.constructPackageName(seed).c_str());
  VarIdVector deps;
  IdPkgRelVector rels;
  m_scope.whatDependsAmongInstalled(seed, deps, rels);
  assert(deps.size() == rels.size());
  for(VarIdVector::size_type i = 0;i < deps.size();i++)
    {
      //      logMsg(LOG_DEBUG, "Processing dependent package \'%s\' with require entry \'%s\'", m_scope.constructPackageName(deps[i]).c_str(), relToString(rels[i]).c_str());
      Clause clause;
      clause.push_back(Lit(seed));
      clause.push_back(Lit(deps[i], 1));
      involvedRemoved.push_back(deps[i]);
      VarIdVector installed;
      m_scope.whatSatisfiesAmongInstalled(rels[i], installed);
      //      logMsg(LOG_DEBUG, "%zu packages satisfy among installed", installed.size());
      for(VarIdVector::size_type k = 0;k < installed.size();k++)
	{
	  //	  logMsg(LOG_DEBUG, "\'%s\' satisfies and installed", m_scope.constructPackageName(installed[k]).c_str());
	  clause.push_back(Lit(installed[k]));
	  involvedRemoved.push_back(installed[k]);
	}
      const VarId replacement = satisfyRequire(rels[i]);
      assert(replacement != BAD_VAR_ID);
      //      logMsg(LOG_DEBUG, "Found default replacement  \'%s\'", m_scope.constructPackageName(replacement).c_str());
      VarIdVector::size_type k;
      for(k = 0;k < installed.size();k++)
	if (replacement == installed[k])
	  break;
      if (k >= installed.size())
	{
	  //	  logMsg(LOG_DEBUG, "Default replacement \'%s\' is not installed, using it", m_scope.constructPackageName(replacement).c_str());
	  clause.push_back(Lit(replacement));
	  involvedInstalled.push_back(replacement);
	} else
	//	logMsg(LOG_DEBUG, "Default replacement \'%s\' is already installed, ignoring it", m_scope.constructPackageName(replacement).c_str());
      m_sat.push_back(clause);
    }
}

void GeneralSolver::processPendings()
{
  while(!m_pendingInstalled.empty() || !m_pendingRemoved.empty())
    {
      logMsg(LOG_DEBUG, "Have pending entries: %zu to be installed and %zu to be removed", m_pendingInstalled.size(), m_pendingRemoved.size());
      while(!m_pendingInstalled.empty())
	{
	  const VarId varId = m_pendingInstalled[m_pendingInstalled.size() - 1];
	  m_pendingInstalled.pop_back();
	  if (m_processedInstalled.find(varId) != m_processedInstalled.end())
	    continue;
	  m_processedInstalled.insert(varId);
	  //	  logMsg(LOG_DEBUG, "Processing pending entry to be installed \'%s\'", m_scope.constructPackageName(varId).c_str());
	  //	  assert(!m_scope.isInstalled(varId));
	}
      while(!m_pendingRemoved.empty())
	{
	  const VarId varId = m_pendingRemoved[m_pendingRemoved.size() - 1];
	  m_pendingRemoved.pop_back();
	  if (m_processedRemoved.find(varId) != m_processedRemoved.end())
	    continue;
	  m_processedRemoved.insert(varId);
	  if (!m_scope.isInstalled(varId))
	    continue;
	  //	  logMsg(LOG_DEBUG, "Processing pending entry to be removed \'%s\'", m_scope.constructPackageName(varId).c_str());
	  VarIdVector involvedInstalled, involvedRemoved;
	  handleChangeToFalse(varId, involvedInstalled, involvedRemoved);
	  for(VarIdVector::size_type i = 0;i < involvedInstalled.size();i++)
	    if (m_processedInstalled.find(involvedInstalled[i]) == m_processedInstalled.end())
	      m_pendingInstalled.push_back(involvedInstalled[i]);
	  for(VarIdVector::size_type i = 0;i < involvedRemoved.size();i++)
	    if (m_processedRemoved.find(involvedRemoved[i]) == m_processedRemoved.end())
	      m_pendingRemoved.push_back(involvedRemoved[i]);
	}
    }
}

VarId GeneralSolver::processPriorityList(const VarIdVector& vars, PackageId provideEntry) const
{
  assert(!vars.empty());
  /*
  //Process the system provide priority list using provideEntry;
  const std::string provideName = m_scope.packageIdToStr(provideEntry);
  StringVector providers;
  m_providePriorityList.getPriority(provideName, providers);
  //Providers vector can easily be empty;
  for(StringVector::size_type i = 0;i < providers.size();i++)
    {
      const PackageId providerId = m_scope.strToPackageId(providers[i]);//FIXME:there can be an error since priority list can return an invalid package name due to its invalid content;
      assert(providerId != BAD_PACKAGE_ID);
      for(VarIdVector::size_type k = 0;k < vars.size();k++)
	if (providerId == m_scope.packageIdOfVarId(vars[k]))
	  return vars[k];
    }
  //No matching entry in priority list;
  return BAD_VAR_ID;
  */
  return BAD_VAR_ID;
}

VarId GeneralSolver::processPriorityBySorting(const VarIdVector& vars) const
{
  assert(!vars.empty());
  //Perform sorting by real package names and take last one;
  PrioritySortItemVector items;
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    items.push_back(PrioritySortItem(vars[i], m_scope.packageIdToStr(m_scope.packageIdOfVarId(vars[i]))));
  std::sort(items.begin(), items.end());
  assert(!items.empty());
  return items[items.size() - 1].varId;
}

std::string GeneralSolver::relToString(const IdPkgRel& rel)
{
  const std::string ver = rel.verString();
  if (ver.empty())
      return m_scope.packageIdToStr(rel.pkgId);
  return m_scope.packageIdToStr(rel.pkgId) + " " + ver;
}

//Static functions;

void printSat(const PackageScope& scope, 
	      const Sat& sat,
	      const StringVector& annotations)
{
  for(Sat::size_type i = 0;i < sat.size();i++)
    {
      const Clause& clause = sat[i];
      if (i < annotations.size())
	std::cout << annotations[i] << std::endl;
      std::cout << "(" << std::endl;
	for(Clause::size_type k = 0;k < clause.size();k++)
	  {
	    const Lit& lit = clause[k];
	    if (lit.neg)
	      std::cout << " !"; else 
	      std::cout << "  ";
	    std::cout << scope.constructPackageNameWithBuildTime(lit.varId);
	    if (k + 1 < clause.size())
	      std::cout << " ||";
	    std::cout << std::endl;
	  }
	std::cout << ")" << std::endl;
	if (i + 1 < sat.size())
	  {
	    std::cout << std::endl;
	    std::cout << "&&" << std::endl;
	    std::cout << std::endl;
	  }
    }
}

void printSolution(const PackageScope& scope,
		   const VarIdVector& install,
		   const VarIdVector& remove)
{
  std::cout << install.size() << " to install, " << remove.size() << " to remove" << std::endl;
  std::cout << std::endl;
  std::cout << "The following packages must be installed:" << std::endl;
  for(size_t k = 0;k < install.size();k++)
    std::cout << scope.constructPackageName(install[k]) << std::endl;
  std::cout << std::endl;
  std::cout << "The following packages must be removed:" << std::endl;
  for(size_t k = 0;k < remove.size();k++)
    std::cout << scope.constructPackageName(remove[k]) << std::endl;
  std::cout << std::endl;
}

