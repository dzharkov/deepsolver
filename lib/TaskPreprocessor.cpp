

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

VarId TaskPreprocessor:::rocessUserTaskItemToInstall(const UserTaskItemToInstall& item)
{
  assert(!item.pkgName.empty());
  const bool hasVersion = !item.version.empty();
  assert(!hasVersion || !item.less || !item.greater);
  const PackageId pkgId = m_scope.strToPackageId(item.pkgName);
  assert(pkgId != BAD_PACKAGE_ID);
  VarIdVector vars;
  //The following line does not take into account available provides;
  m_scope.selectMatchingVars(pkgId, vars);
  if (!vars.empty())
    {
      const VarId res = m_scope.selectTheNewest(vars);
      assert(res != BAD_VAR_ID);
      return res;
    }
  if (hasVersion)
    {

    }
}
