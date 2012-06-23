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
#include"ProvideMap.h"
#include"InstalledReferences.h"

//All methods can throw only SystemException and RpmException;
//Vars is the indices in the main PackageScopeContent array;
class PackageScope 
{
public:
  PackageScope(const PackageScopeContent& content,
	       const ProvideMap& provideMap,
	       const InstalledReferences& requiresReferences,
	       const InstalledReferences& conflictsReferences)
    : m_content(content), 
      m_provideMap(provideMap),
      m_requiresReferences(requiresReferences),
      m_conflictsReferences(conflictsReferences) {}

  virtual ~PackageScope() {}

public:
  PackageId packageIdOfVarId(VarId varId) const;
  std::string constructPackageName(VarId varId) const;
  std::string constructFullVersion(VarId varId) const;
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

  void selectMatchingVarsNoProvides(PackageId packageId, VarIdVector& vars);
  void selectMatchingVarsNoProvides(PackageId packageId, const VersionCond& ver, VarIdVector& vars);

  void selectMatchingVarsWithProvides(PackageId packageId, VarIdVector& vars);
  void selectMatchingVarsWithProvides(PackageId packageId, const VersionCond& ver, VarIdVector& vars);

  void selectMatchingVarsAmongProvides(PackageId packageId, VarIdVector& vars);
  void selectMatchingVarsAmongProvides(PackageId packageId, const VersionCond& ver, VarIdVector& vars);

  bool isInstalled(VarId varId) const;
  void selectInstalledNoProvides(PackageId pkgId, VarIdVector& vars) const;

  void selectTheNewest(VarIdVector& vars);
  void selectTheNewestByProvide(VarIdVector& vars, PackageId provideEntry);
  bool allProvidesHaveTheVersion(const VarIdVector& vars, PackageId provideEntry);

  void getRequires(VarId varId, PackageIdVector& depWithoutVersion, PackageIdVector& depWithVersion, VersionCondVector& versions) const;
  void getConflicts(VarId varId, PackageIdVector& withoutVersion, PackageIdVector& withVersion, VersionCondVector& versions) const;

  bool canBeSatisfiedByInstalled(PackageId pkgId);
  bool canBeSatisfiedByInstalled(PackageId pkgId, const VersionCond& ver);

  void whatDependsAmongINstalled(VarId varId, VarIdVector& res, IdPkgRelVector& resRels);
  void whatConflictsAmongINstalled(VarId varId, VarIdVector& res, IdPkgRelVector& resRels);

private:
  typedef PackageScopeContent::PkgInfo PkgInfo;
  typedef PackageScopeContent::RelInfo RelInfo;
  typedef PackageScopeContent::PkgInfoVector PkgInfoVector;
  typedef PackageScopeContent::RelInfoVector RelInfoVector;

private:
  const PackageScopeContent& m_content;
  const ProvideMap& m_provideMap;
  const InstalledReferences& m_requiresReferences;
  const InstalledReferences& m_conflictsReferences; 
}; //class PackageScope; 

#endif //DEEPSOLVER_PACKAGE_SCOPE_H;
