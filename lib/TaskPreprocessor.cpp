

#include"deepsolver.h"
#include"TaskPreprocessor.h"
#include"TaskException.h"
#include"version.h"

#define TASK_STOP(x) throw TaskException(x)

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

template<typename T>
void removeDublications(std::vector<T>& v)
{
  //FIXME:
}

void TaskPreprocessor::preprocess(const UserTask& userTask,
				  VarIdVector& strongToInstall,
				  VarIdVector& strongToRemove)
{
  strongToInstall.clear();
  strongToRemove.clear();
  /*
   * First of all adding to scope all packages newly temporarily available;
   * the set of temporary packages must be definitely install;
   */
  for(StringSet::const_iterator it = userTask.urlsToInstall.begin(); it != userTask.urlsToInstall.end();it++) 
    strongToInstall.push_back(m_scope.registerTemporarily(*it));
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
	TASK_STOP("\'" + *it + "\' is not a known package name");
      VarIdVector vars;
      //The following line does not take into account provide entries;
      m_scope.selectMatchingVars(pkgId, vars);
      for(VarIdVector::size_type k = 0;k < vars.size();k++)
	strongToRemove.push_back(vars[k]);
    }
  //For every package to install we must prohibit all other versions except selected;
  //FIXME:
  //The same package cannot be included both into strongToInstall and strongToRemove;
  for(VarIdVector::size_type i1 = 0;i1 < strongToInstall.size();i1++)
    for(VarIdVector::size_type i2 = 0;i2 < strongToRemove.size();i2++)
      if (strongToInstall[i1] == strongToRemove[i2])
	{
	  const std::string pkgName = m_scope.packageIdToStr(strongToInstall[i1]);
	  assert(!pkgName.empty());
	  TASK_STOP("User task requires the impossible solution: \'" + pkgName + "\' was selected to be installed and to be removed simultaneously");
	}
  removeDublications(strongToInstall);
  removeDublications(strongToRemove);
}

void TaskPreprocessor::buildDepClosure(VarId varId, VarIdSet& required, VarIdSet& conflicted)
{
  VarIdSet processed;
  VarIdVector pending;
  processed.insert(varId);
  pending.push_back(varId);
  while(!pending.empty())
    {
      const VarId varId = pending[pending.size() - 1];
      pending.pop_back();
      VarIdVector depending;
      processRequires(varId, depending);
      for(VarIdVector::size_type i = 0;i < depending.size();i++)
	{
	  if (processed.find(depending[i]) != processed.end())
	    continue;
	  required.insert(depending[i]);
	  processed.insert(depending[i]);
	  pending.push_back(depending[i]);
	}
      depending.clear();
      processConflicts(varId, depending);
      for(VarIdVector::size_type i = 0;i < depending.size();i++)
	conflicted.insert(depending[i]);
    } //while(!pending.empty());
}

void TaskPreprocessor::processRequires(VarId varId, VarIdVector& dependent)
{
  PackageIdVector depWithoutVersion, depWithVersion;
  VersionCondVector versions;
  m_scope.getRequires(varId, depWithoutVersion, depWithVersion, versions);
  assert(depWithVersion.size() == versions.size());
  for(PackageIdVector::size_type i = 0;i < depWithoutVersion.size();i++)
    {
      const VarId res = processRequireWithoutVersion(depWithoutVersion[i]);
      assert(res != BAD_VAR_ID);
      dependent.push_back(res);
    }
  for(PackageIdVector::size_type i = 0;i < depWithVersion.size();i++)
    {
      const VarId res = processRequireWithVersion(depWithVersion[i], versions[i]);
      assert(res != BAD_VAR_ID);
      dependent.push_back(res);
    }
}

void TaskPreprocessor::processConflicts(VarId varId, VarIdVector& res)
{
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
      m_scope.selectMatchingWithVersionVarsWithProvides(withVersion[i], versions[i], vars);
      for(VarIdVector::size_type i = 0;i < vars.size();i++)
	if (vars[i] != varId)//Package cannot conflict with itself;
	  res.push_back(vars[i]);
    }
}

VarId TaskPreprocessor::processUserTaskItemToInstall(const UserTaskItemToInstall& item)
{
  assert(!item.pkgName.empty());
  const bool hasVersion = !item.version.empty();
  assert(!hasVersion || !item.less || !item.greater);
  if (!m_scope.checkName(item.pkgName))
    TASK_STOP("\'" + item.pkgName + "\' is not a known package name");
  const PackageId pkgId = m_scope.strToPackageId(item.pkgName);
  assert(pkgId != BAD_PACKAGE_ID);
  VarIdVector vars;
  if (!hasVersion)
    {
      //The following line does not take into account available provides;
      m_scope.selectMatchingVars(pkgId, vars);
    } else
    {
      VersionCond ver(item.version, item.less, item.equals, item.greater);
      //This line does not handle provides too;
      m_scope.selectMatchingWithVersionVars(pkgId, ver, vars);
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
   * name at all or there are inappropriate version restrictions ), so 
   * now the time to select anything among presented provides records;
   */
  if (hasVersion)
    {
      VersionCond ver(item.version, item.less, item.equals, item.greater);
      m_scope.selectMatchingWithVersionVarsAmongProvides(pkgId, ver, vars);
    } else
    m_scope.selectMatchingVarsAmongProvides(pkgId, vars);
  if (vars.empty())//No appropriate packages at all;
    TASK_STOP("\'" + item.makeStr() + "\' has no installation candidat");
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

VarId TaskPreprocessor::processRequireWithoutVersion(PackageId pkgId)
{
  assert(pkgId != BAD_PACKAGE_ID);
  VarIdVector vars;
  //The following line does not take into account available provides;
  m_scope.selectMatchingVars(pkgId, vars);
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
    TASK_STOP("Require entry \'" + m_scope.packageIdToStr(pkgId) + "\' has no installation candidat");
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

VarId TaskPreprocessor::processRequireWithVersion(PackageId pkgId, const VersionCond& cond)
{
  assert(pkgId != BAD_PACKAGE_ID);
  VarIdVector vars;
  //This line does not handle provides ;
  m_scope.selectMatchingWithVersionVars(pkgId, cond, vars);
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
  m_scope.selectMatchingWithVersionVarsAmongProvides(pkgId, cond, vars);
  if (vars.empty())//No appropriate packages at all;
    TASK_STOP("Require entry \'" + m_scope.packageIdToStr(pkgId) + "\' with version has no installation candidat");
  const VarId res = processPriorityList(vars, pkgId);
  if (res != BAD_VAR_ID)
    return res;
  m_scope.selectTheNewestByProvide(vars, pkgId);
  assert(!vars.empty());
  if (vars.size() == 1)
    return vars.front();
  return processPriorityBySorting(vars);
}

VarId TaskPreprocessor::processPriorityList(const VarIdVector& vars, PackageId provideEntry) const
{
  assert(!vars.empty());
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
}

VarId TaskPreprocessor::processPriorityBySorting(const VarIdVector& vars) const
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
