
#ifndef DEPSOLVER_CONFIG_CENTER_H
#define DEPSOLVER_CONFIG_CENTER_H

#include"DefaultValues.h"

class ConfigCenter
{
public:
  ConfigCenter():
    configDirPath(DEFAULT_CONFIG_DIR)
  {}

public:
  std::string configDirPath;
  std::string attachedRepoList;
}; //class ConfigCenter;

#endif //DEPSOLVER_CONFIG_CENTER_H;
