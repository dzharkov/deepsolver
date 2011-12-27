
#ifndef DEPSOLVER_PACKAGE_SCOPE_H
#define DEPSOLVER_PACKAGE_SCOPE_H
//All methods can throw only SystemException;
class PackageScope 
{
public:
  PackageScope() {}
  virtual ~PackageScope() {}

public:
  PackageId strToPackageId(const std::string& name) const;
  std::string packageNameToStr(PackageId packageId) const;

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

  //Only by real names without provides;
  void selectMatchingVars(PackageId packageId, VarIdVector& vars);
  //Only by real names but respecting version info;
  void selectMatchingWithVersionVars(PackageId packageId, const VersionCond& ver, VarIdVector& vars);
  //By real names and provides;
  void selectMatchingVarsWithProvides(PackageId packageId, VarIdVector& vars);
  //Only by provides;
  void selectMatchingVarsAmongProvides(PackageId packageId, VarIdVector& vars);
  //Only by provides but respecting version info (entries without version must be silently skipped);
  void selectMatchingWithVersionVarsAmongProvides(PackageId packageId, const VersionCond& ver, VarIdVector& vars);

  //Only by real names;
  bool isINstalled(PackageId packageId) const;
  //By real name and provides;
  bool isInstallWithProvides(PackageId packageId);

  //Only by versions of real names;
  void selectTheNewest(VarIdVector& vars);
  //Only by versions of exact provide entry;
  void selectTheNewestByProvide(VarIdVector& vars, PackageId provideEntry);



  bool allProvidesHaveTheVersion(const VarIdVector& vars, PackageId provideEntry);

  //  void whatRequiresAmongInstalled();
}; //class PackageScope; 

#endif //DEPSOLVER_PACKAGE_SCOPE_H;
