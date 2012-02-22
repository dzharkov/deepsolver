
#ifndef DEEPSOLVER_PACKAGE_SCOPE_CONTENT_LOADER_H
#define DEEPSOLVER_PACKAGE_SCOPE_CONTENT_LOADER_H

#include"PackageScopeContent.h"

class PackageScopeContentLoader: public PackageScopeContent
{
public:
  PackageScopeContentLoader() {}
  virtual ~PackageScopeContentLoader() {}

public:
  void loadFromFile(const std::string& fileName);

}; //class PackageScopeContentLoader;

#endif //DEEPSOLVER_PACKAGE_SCOPE_CONTENT_LOADER_H;
