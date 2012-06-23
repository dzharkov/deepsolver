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
#include"version.h"

static void printSolution(const PackageScope& scope,
		   const VarIdVector& install,
		   const VarIdVector& remove,
		   const VarIdToVarIdMap& upgrade)
{
  std::cout << install.size() << " to install, " << remove.size() << " to remove, " << upgrade.size() << " to upgrade" << std::endl;
  std::cout << std::endl;
  std::cout << "The following packages must be installed:" << std::endl;
  for(size_t k = 0;k < install.size();k++)
    std::cout << scope.constructPackageName(install[k]) << std::endl;
  std::cout << std::endl;
  std::cout << "The following packages must be removed:" << std::endl;
  for(size_t k = 0;k < remove.size();k++)
    std::cout << scope.constructPackageName(remove[k]) << std::endl;
  std::cout << std::endl;
  std::cout << "The following packages must be upgraded:" << std::endl;
  for(VarIdToVarIdMap::const_iterator it = upgrade.begin();it != upgrade.end();it++)
    std::cout << scope.constructPackageName(it->first) << " -> " << scope.constructPackageName(it->second) << std::endl;
}

template<typename T>
void removeDublications(std::vector<T>& v)
{
  //Be careful: only for items with operator < and operator >;
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

class StrictSolver: public AbstractTaskSolver
{
public:
  StrictSolver(const PackageScopeContent& content,
	       const ProvideMap& provideMap,
	       const InstalledReferences& requiresReferences,
	       const InstalledReferences& conflictsReferences)
    : m_content(content) , m_scope(content, provideMap, requiresReferences, conflictsReferences) {}

  virtual ~StrictSolver() {}

public:
  void solve(const UserTask& task, VarIdVector& toInstall, VarIdVector& toRemove, VarIdToVarIdMap& toUpgrade);

private:
  void translateUserTask(const UserTask& userTask);
  void walkThroughRequires(VarId startFrom, VarIdVector& mustBeInstalled, VarIdVector& mayBeInstalled);
  VarId translateItemToInstall(const UserTaskItemToInstall& item);
  VarId satisfyRequire(PackageId pkgId);
  VarId satisfyRequire(PackageId pkgId, const VersionCond& version);
  void isValidTask() const;
  VarId processPriorityList(const VarIdVector& vars, PackageId provideEntry) const;
  VarId processPriorityBySorting(const VarIdVector& vars) const;
  void notToInstallButToUpgrade(const VarIdVector& vars, VarIdVector& toInstall, VarIdToVarIdMap& toUpgrade);
  void findAllConflictedVars(VarId varId, VarIdVector& res);

private:
  const PackageScopeContent& m_content;
  PackageScope m_scope; 
  VarIdVector m_userTaskInstall, m_userTaskRemove, m_anywayInstall, m_anywayRemove;
  VarIdToVarIdMap m_userTaskUpgrade, m_anywayUpgrade;
}; //class StrictSolver;

std::auto_ptr<AbstractTaskSolver> createStrictTaskSolver(const PackageScopeContent& content,
							 const ProvideMap& provideMap,
							 const InstalledReferences& requiresReferences,
							 const InstalledReferences& conflictsReferences)
{
  logMsg(LOG_DEBUG, "Creating strict task solver");
  return std::auto_ptr<AbstractTaskSolver>(new StrictSolver(content, provideMap, requiresReferences, conflictsReferences));
}

void StrictSolver::solve(const UserTask& task, VarIdVector& toInstall, VarIdVector& toRemove, VarIdToVarIdMap& toUpgrade)
{
  m_userTaskInstall.clear();
  m_userTaskRemove.clear();
  m_userTaskUpgrade.clear();
  m_anywayInstall.clear();
  m_anywayRemove.clear();
  m_anywayUpgrade.clear();
  translateUserTask(task);
  //  isValidTask();
  logMsg(LOG_DEBUG, "User task translated: %zu to install, %zu to remove, %zu to upgrade", m_userTaskInstall.size(), m_userTaskRemove.size(), m_userTaskUpgrade.size());
  for(VarIdVector::size_type i = 0;i < m_userTaskInstall.size();i++)
    {
      logMsg(LOG_DEBUG, "install: %s", m_scope.constructPackageName(m_userTaskInstall[i]).c_str());
      m_anywayInstall.push_back(m_userTaskInstall[i]);
    }
  for(VarIdVector::size_type i = 0;i < m_userTaskRemove.size();i++)
    {
      logMsg(LOG_DEBUG, "remove: %s", m_scope.constructPackageName(m_userTaskRemove[i]).c_str());
      m_anywayRemove.push_back(m_userTaskRemove[i]);
    }
  for(VarIdToVarIdMap::const_iterator it = m_userTaskUpgrade.begin();it != m_userTaskUpgrade.end();it++)
    {
    logMsg(LOG_DEBUG, "upgrade: %s -> %s", m_scope.constructPackageName(it->first).c_str(), m_scope.constructPackageName(it->second).c_str());
    m_anywayUpgrade.insert(VarIdToVarIdMap::value_type(it->first, it->second));
    }
  //Extracting all requires;
  for(VarIdVector::size_type i = 0;i < m_userTaskInstall.size();i++)
    {
      VarIdVector must, may;
      walkThroughRequires(m_userTaskInstall[i], must, may);
      removeDublications(must);
      removeDublications(may);
      notToInstallButToUpgrade(must, m_anywayInstall, m_anywayUpgrade);
      //FIXME:may be installed;
    }
  //The same for packages to be upgraded;
  for(VarIdToVarIdMap::const_iterator it = m_userTaskUpgrade.begin();it != m_userTaskUpgrade.end();it++)
    {
      VarIdVector must, may;
      walkThroughRequires(it->first, must, may);
      removeDublications(must);
      removeDublications(may);
      notToInstallButToUpgrade(must, m_anywayInstall, m_anywayUpgrade);
      //FIXME:may be installed;
    }
  removeDublications(m_anywayInstall);
  /*
   * Here m_anywayInstall and m_anywayUpgrade have their final state;
   * Building list of all conflicted variables;
   */
  for(VarIdVector::size_type i = 0;i < m_anywayInstall.size();i++)
      findAllConflictedVars(m_anywayInstall[i], m_anywayRemove);
  for(VarIdToVarIdMap::const_iterator it = m_anywayUpgrade.begin();it != m_anywayUpgrade.end();it++)
      findAllConflictedVars(it->first, m_anywayRemove);
  removeDublications(m_anywayRemove);

  printSolution(m_scope, m_anywayInstall, m_anywayRemove, m_anywayUpgrade);
}

void StrictSolver::translateUserTask(const UserTask& userTask)
{
  m_userTaskInstall.clear();
  m_userTaskRemove.clear();
  m_userTaskUpgrade.clear();
  for(UserTaskItemToInstallVector::size_type i = 0;i < userTask.itemsToInstall.size();i++)
    {
      //The following line checks the entire package scope including installed packages, so it can return also the installed package if it matches the user request;
      const VarId varId = translateItemToInstall(userTask.itemsToInstall[i]);
      assert(varId != BAD_VAR_ID);
      m_userTaskInstall.push_back(varId);
    }
  for(StringSet::const_iterator it = userTask.namesToRemove.begin() ;it != userTask.namesToRemove.end();it++)
    {
      const PackageId pkgId = m_scope.strToPackageId(*it);
      if (pkgId == BAD_PACKAGE_ID)
	{
	  std::cout << "requested to remove package \'" << *it << "\' is not installed" << std::endl;//FIXME:proper user notification;
	  logMsg(LOG_DEBUG, "Request contains ask to remove unknown package \'%s\', skipping", it->c_str());
	  continue;
	}
      VarIdVector vars;
      m_scope.selectInstalledNoProvides(pkgId, vars);
      if (vars.empty())
	{
	  std::cout << "requested to remove package \'" << *it << "\' is not installed" << std::endl;//FIXME:proper user notification;
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
	  std::cout << m_scope.constructPackageName(m_userTaskInstall[i]) << " is the newest version" << std::endl;//FIXME:Proper user notification;
	  logMsg(LOG_DEBUG, "Accepted to install variable %zu is already installed, skipping", m_userTaskInstall[i]);
	  continue;
	}
      v.push_back(m_userTaskInstall[i]);
    }
  m_userTaskInstall.clear();
  for(VarIdVector::size_type i = 0;i < v.size();i++)//FIXME:This for was extracted to separate method and here must be just its call;
    {
      const VarId varId = v[i];
      const PackageId pkgId = m_scope.packageIdOfVarId(varId);
      assert(pkgId != BAD_PACKAGE_ID);
      VarIdVector installed;
      m_scope.selectInstalledNoProvides(pkgId, installed);
      if (installed.size() > 1)
	logMsg(LOG_WARNING, "Package \'%s\' is installed more than once", m_scope.packageIdToStr(pkgId).c_str());
      if (installed.size() >= 1)
	{
	  assert(m_userTaskUpgrade.find(installed[0]) == m_userTaskUpgrade.end());//FIXME:Actually we should handle it more carefully;
	  m_userTaskUpgrade.insert(VarIdToVarIdMap::value_type(installed[0], varId));
	} else
	m_userTaskInstall.push_back(varId);
    }
  removeDublications(m_userTaskInstall);
  removeDublications(m_userTaskRemove);
}

void StrictSolver::walkThroughRequires(VarId startFrom, VarIdVector& mustBeInstalled, VarIdVector& mayBeInstalled)
{
  assert(startFrom != BAD_VAR_ID);
  VarIdSet processed;
  VarIdVector pending;
  pending.push_back(startFrom);
  while(!pending.empty())
    {
      const VarId varId = pending[pending.size() - 1];
      pending.pop_back();
      processed.insert(varId);
      PackageIdVector depWithoutVersion, depWithVersion;
      VersionCondVector versions;
      m_scope.getRequires(startFrom, depWithoutVersion, depWithVersion, versions);
      assert(depWithVersion.size() == versions.size());
      for(PackageIdVector::size_type i = 0;i < depWithoutVersion.size();i++)
	{
	  const VarId dep = satisfyRequire(depWithoutVersion[i]);
	  if (dep == startFrom)//explicit check to be sure, assuming startFrom must be installed anyway;
	    continue;
	  if (m_scope.isInstalled(dep))//it is already installed, never mind;
	    continue;
	  if (!m_scope.canBeSatisfiedByInstalled(depWithoutVersion[i]))
	    {
	      //This package is strongly required, processing;
	      if (processed.find(dep) != processed.end())
		continue;
	      mustBeInstalled.push_back(dep);
	      pending.push_back(dep);
	    } else 
	    mayBeInstalled.push_back(dep);//This package is not strongly required to be installed, so marking it as just possible;
	} //for(depWithoutVersion);
      for(PackageIdVector::size_type i = 0;i < depWithVersion.size();i++)
	{
	  const VarId dep = satisfyRequire(depWithVersion[i], versions[i]);
	  if (dep == startFrom)//explicit check to be sure, assuming startFrom must be installed anyway;
	    continue;
	  if (m_scope.isInstalled(dep))//it is already installed, never mind;
	    continue;
	  if (!m_scope.canBeSatisfiedByInstalled(depWithVersion[i], versions[i]))
	    {
	      //This package is strongly required, processing;
	      if (processed.find(dep) != processed.end())
		continue;
	      mustBeInstalled.push_back(dep);
	      pending.push_back(dep);
	    } else 
	    mayBeInstalled.push_back(dep);//This package is not strongly required to be installed, so marking it as just possible;
	} //for(depWithoutVersion);
    } //while(pending);
}

void StrictSolver::findAllConflictedVars(VarId varId, VarIdVector& res)
{
  //Do not clear res here!!!
  PackageIdVector withoutVersion, withVersion;
  VersionCondVector versions;
  m_scope.getConflicts(varId, withoutVersion, withVersion, versions);
  assert(withVersion.size() == versions.size());
  for(PackageIdVector::size_type i = 0;i < withoutVersion.size();i++)
    {
      VarIdVector vars;
      m_scope.selectMatchingVarsWithProvides(withoutVersion[i], vars);
      for(VarIdVector::size_type i = 0;i < vars.size();i++)
	if (vars[i] != varId)//Package cannot conflict with itself;
	  res.push_back(vars[i]);
    }
  for(PackageIdVector::size_type i = 0;i < withVersion.size();i++)
    {
      VarIdVector vars;
      m_scope.selectMatchingVarsWithProvides(withVersion[i], versions[i], vars);
      for(VarIdVector::size_type i = 0;i < vars.size();i++)
	if (vars[i] != varId)//Package cannot conflict with itself;
	  res.push_back(vars[i]);
    }
  //Here we check are there any conflicts from the installed packages;
  VarIdVector vars;
  IdPkgRelVector rels;
  m_scope.whatConflictsAmongInstalled(varId, vars, rels);
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    res.push_back(vars[i]);
}

VarId StrictSolver::translateItemToInstall(const UserTaskItemToInstall& item) 
{
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

VarId StrictSolver::satisfyRequire(PackageId pkgId)
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
    throw TaskException(TaskErrorNoInstallationCandidat, m_scope.packageIdToStr(pkgId));
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

VarId StrictSolver::satisfyRequire(PackageId pkgId, const VersionCond& version)
{
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
    throw TaskException(TaskErrorNoInstallationCandidat, m_scope.packageIdToStr(pkgId));
  const VarId res = processPriorityList(vars, pkgId);
  if (res != BAD_VAR_ID)
    return res;
  m_scope.selectTheNewestByProvide(vars, pkgId);
  assert(!vars.empty());
  if (vars.size() == 1)
    return vars.front();
  return processPriorityBySorting(vars);
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

void StrictSolver::notToInstallButToUpgrade(const VarIdVector& vars, VarIdVector& toInstall, VarIdToVarIdMap& toUpgrade)
{
  //Do not clear any of result structures here!!!
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    {
      const VarId varId = vars[i];
      const PackageId pkgId = m_scope.packageIdOfVarId(varId);
      assert(pkgId != BAD_PACKAGE_ID);
      VarIdVector installed;
      m_scope.selectInstalledNoProvides(pkgId, installed);
      if (installed.size() > 1)
	logMsg(LOG_WARNING, "Package \'%s\' is installed more than once", m_scope.packageIdToStr(pkgId).c_str());
      if (installed.size() >= 1)
	{
	  assert(toUpgrade.find(installed[0]) == toUpgrade.end());//FIXME:Actually we should handle it more carefully;
	  toUpgrade.insert(VarIdToVarIdMap::value_type(installed[0], varId));
	} else
	toInstall.push_back(varId);
    }
}

VarId StrictSolver::processPriorityList(const VarIdVector& vars, PackageId provideEntry) const
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

VarId StrictSolver::processPriorityBySorting(const VarIdVector& vars) const
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
