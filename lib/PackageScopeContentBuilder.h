
#ifndef DEEPSOLVER_PACKAGE_SCOPE_CONTENT_BUILDER_H
#define DEEPSOLVER_PACKAGE_SCOPE_CONTENT_BUILDER_H

#include"PackageScopeContent.h"

class PackageScopeContentBuilder: public PackageScopeContent
{
public:
  PackageScopeContentBuilder() {}
  virtual ~PackageScopeContentBuilder() {}

public:
  void addPkg(const PkgFile& pkgFile);
  void addProvideMapItem(const std::string& provideName, const std::string& packageName);
  void commit();

private:
  void processRels(const NamedPkgRelVector& rels, size_t& pos, size_t& count);
  PackageId registerName(const std::string& name);
}; //class PackageScopeContentBuilder;

#endif //DEEPSOLVER_PACKAGE_SCOPE_CONTENT_BUILDER_H;
