

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
}
