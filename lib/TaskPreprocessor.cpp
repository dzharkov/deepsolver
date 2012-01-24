

#include"depsolver.h"
#include"TaskPreprocessor.h"

static void TASK_STOP(const std::string& msg)//FIXME:
{
  std::cerr << msg << std::endl;
  assert(0);
}

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

VarId TaskPreprocessor::processUserTaskItemToInstall(const UserTaskItemToInstall& item)
{
  assert(!item.pkgName.empty());
  const bool hasVersion = !item.version.empty();
  assert(!hasVersion || !item.less || !item.greater);
  const PackageId pkgId = m_scope.strToPackageId(item.pkgName);
if (pkgId == BAD_PACKAGE_ID)
  TASK_STOP("\'" + item.pkgName + "\' is not a known package name");
  VarIdVector vars;
  if (!hasVersion)
    {
      //The following line does not take into account available provides;
      m_scope.selectMatchingVars(pkgId, vars);
    } else
    {
      VersionCond ver(item.version, item.less, item.equals, item.greater);
      //This line does not handle provides too;;
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
      m_scope.selectTheNewestByProvide(vars, pkgId);
      assert(!vars.empty());
      if (vars.size() == 1)
	return vars.front();
      return processPriority(vars, pkgId);
    }
      return processPriority(vars, pkgId);
}

VarId TaskPreprocessor::processPriority(const VarIdVector& vars, PackageId provideEntry) const
{
  //Process the system provide priority list using provideEntry;
  //Perform sorting by real package names and take last;
  assert(!vars.empty());
  return vars[0];
  assert(0);//FIXME:
  return BAD_VAR_ID;
}
