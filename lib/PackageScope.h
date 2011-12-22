
#ifndef DEPSOLVER_PACKAGE_SCOPE_H
#define DEPSOLVER_PACKAGE_SCOPE_H

class PackageScope 
{
public:
  PackageScope() {}
  virtual ~PackageScope() {}

public:
  VarId registerTemporarily(const std::string& url); 
  void whatRequiresAmongInstalled();
}; //class PackageScope; 

#endif //DEPSOLVER_PACKAGE_SCOPE_H;
