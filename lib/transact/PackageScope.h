/*
   Copyright 2011-2012 ALT Linux
   Copyright 2011-2012 Michael Pozhidaev

   This file is part of the Deepsolver.

   Deepsolver is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   Deepsolver is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
*/

#ifndef DEEPSOLVER_PACKAGE_SCOPE_H
#define DEEPSOLVER_PACKAGE_SCOPE_H

#include"Pkg.h"
#include"PackageScopeContent.h"

//All methods can throw only SystemException and RpmException;
//Vars is the indices in the main PackageScopeContent array;
class PackageScope 
{
public:
  PackageScope(const PackageScopeContent& content)
    : m_content(content) {}

  virtual ~PackageScope() {}

public:
  PackageId packageIdOfVarId(VarId varId) const;
  std::string constructPackageName(VarId varId) const;
  /**\brief FIXME*/
  bool checkName(const std::string& name) const;

  /**\brief Translates package name as a string to PackageId value
   *
   * Package Id replaces both the real names and provides entries
   */
  PackageId strToPackageId(const std::string& name) const;

  /**\brief Translates the package name replaced by PackageId value to original string
   *
   * Package Id replaces both the real names and provides entries
   */
  std::string packageIdToStr(PackageId packageId) const;

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
  //By real names and provides and with version respecting;
  void selectMatchingWithVersionVarsWithProvides(PackageId packageId, const VersionCond& ver, VarIdVector& vars);
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

  /**\brief FIXME*/
  void getRequires(VarId varId, PackageIdVector& depWithoutVersion, PackageIdVector& depWithVersion, VersionCondVector& versions) const;
  void getConflicts(VarId varId, PackageIdVector& withoutVersion, PackageIdVector& withVersion, VersionCondVector& versions) const;

  //if pkgId is not BAD_PACKAGE_ID, new package must be registered with the specified value;
  void registerInstalledPackage(const Pkg& pkg, PackageId pkgId);

  //  void whatRequiresAmongInstalled();

private:
  const PackageScopeContent& m_content;
}; //class PackageScope; 

#endif //DEEPSOLVER_PACKAGE_SCOPE_H;
