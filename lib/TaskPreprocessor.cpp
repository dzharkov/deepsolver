

#include"depsolver.h"
#include"TaskPreprocessor.h"

void TaskPreprocessor::preprocess(const UserTask& userTask)
{
  VarIdVector strongInstall, strongRemove;
  /*
   * First of all adding to scope all packages newly temporarily available;
   * The set of temporary packages must be definitely install;
   */
  for(StringSet::const_iterator it = userTask.urlsToInstall.begin(); it != userTask.urlsToInstall.end();it++) 
    strongInstall.push_back(m_scope.registerTemporarily(*it));
  //FIXME:downgrade;
}

VarId TaskPreprocessor::processUserTaskItemToInstall(const UserTaskItemToInstall& item)
{
  assert(!item.pkgName.empty());
  const bool hasVersion = !item.version.empty();
  assert(!hasVersion || !item.less || !item.greater);
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
}
