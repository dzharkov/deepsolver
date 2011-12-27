
#ifndef DEPSOLVER_PACKAGE_SCOPE_H
#define DEPSOLVER_PACKAGE_SCOPE_H

class PackageScope 
{
public:
  PackageScope() {}
  virtual ~PackageScope() {}

public:
  /**\brief Adds to the database the package not presented in available scope
   *
   * This method adds to the current package database new record as a
   * reference to the external file from the local hard disk or from the
   * network. if the package being registering is already present among of
   * set of known packages (it has the same name, epoch, version, release
   * and build time as known package) the known package must be replaced by
   * new one since temporary package must have higher priority in install
   * operations.
   *
   * \param [in] url The reference to the package file to register
   *
   * \return The identifier of newly added package to scope
   */
  VarId registerTemporarily(const std::string& url); 

  void selectVarIdByRealName(PackageId packageId, VarIdVector& varIdVector);
  void whatRequiresAmongInstalled();
}; //class PackageScope; 

#endif //DEPSOLVER_PACKAGE_SCOPE_H;
