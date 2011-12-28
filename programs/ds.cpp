
#include"depsolver.h"
#include"TaskPreprocessor.h"
#include"SatConstructor.h"
#include"sat/Sat2.h"

int main(int argc, char* argv[])
{
  UserTask task;
  //User task initialization;
  VarIdVector strongToINstall, strongToRemove;
  PackageScope scope;
  //FIXME://Package scope initialization;
  TaskPreprocessor taskPreprocessor(scope);
  taskPreprocessor.preprocess(task, strongToInstall, strongToRemove);
  Sat2 sat;
  SatConstructor satConstructor(scope);
  satConstructor.construct(strongToInstall, strongToRemove, sat);
  //Solving;
  return 0;
}
