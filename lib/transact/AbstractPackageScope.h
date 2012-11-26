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

#ifndef DEEPSOLVER_ABSTRACT_PACKAGE_SCOPE_H
#define DEEPSOLVER_ABSTRACT_PACKAGE_SCOPE_H

class AbstractPackageScope
{
public:
  AbstractPackageScope() {}
  virtual ~AbstractPackageScope() {}

public:
  bool allProvidesHaveTheVersion(const VarIdVector& vars, PackageId provideEntry) = 0;
  bool checkName(const std::string& name) = 0;
  std::string constructPackageName(VarId varId) = 0;
  std::string constructPackageNameWithBuildTime(VarId varId) = 0;
  void getConflicts(VarId varId, IdPkgRelVector& res) = 0;
  void getRequires(VarId varId, IdPkgRelVector& res) = 0;
  bool isInstalled(VarId varId) = 0;
  PackageId packageIdOfVarId(VarId varId) = 0;
  std::string packageIdToStr(PackageId packageId) = 0;
  void selectMatchingVarsAmongProvides(const IdPkgRel& rel, VarIdVector& vars) = 0;
  void selectMatchingVarsAmongProvides(PackageId packageId, VarIdVector& vars) = 0;
  void selectMatchingVarsAmongProvides(PackageId packageId, const VersionCond& ver, VarIdVector& vars) = 0;
  void selectMatchingVarsRealNames(const IdPkgRel& rel, VarIdVector& vars) = 0;
  void selectMatchingVarsRealNames(PackageId packageId, VarIdVector& vars) = 0;
  void selectMatchingVarsRealNames(PackageId packageId, const VersionCond& ver, VarIdVector& vars) = 0;
  void selectMatchingVarsWithProvides(const IdPkgRel& rel, VarIdVector& vars) = 0;
  void selectMatchingVarsWithProvides(PackageId packageId, VarIdVector& vars) = 0;
  void selectMatchingVarsWithProvides(PackageId packageId, const VersionCond& ver, VarIdVector& vars) = 0;
  void selectTheNewest(VarIdVector& vars) = 0;
  void selectTheNewestByProvide(VarIdVector& vars, PackageId provideEntry) = 0;
  PackageId strToPackageId(const std::string& name) = 0;
  void whatConflictsAmongInstalled(VarId varId, VarIdVector& res, IdPkgRelVector& resRels) = 0;
  void whatDependsAmongInstalled(VarId varId, VarIdVector& res, IdPkgRelVector& resRels) = 0;
  void whatSatisfiesAmongInstalled(const IdPkgRel& rel, VarIdVector& res) = 0;
}; //class AbstractPackageScope;

#endif //DEEPSOLVER_ABSTRACT_PACKAGE_SCOPE_H;
