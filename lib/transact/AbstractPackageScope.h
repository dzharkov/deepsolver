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
  bool allProvidesHaveTheVersion(const VarIdVector& vars, PackageId provideEntry);
  bool checkName(const std::string& name);
  std::string constructPackageName(VarId varId);
  std::string constructPackageNameWithBuildTime(VarId varId);
  void getConflicts(VarId varId, IdPkgRelVector& res);
  void getRequires(VarId varId, IdPkgRelVector& res);
  bool isInstalled(VarId varId);
  PackageId packageIdOfVarId(VarId varId);
  std::string packageIdToStr(PackageId packageId);
  void selectMatchingVarsAmongProvides(PackageId packageId, const VersionCond& ver, VarIdVector& vars);
  void selectMatchingVarsAmongProvides(PackageId packageId, VarIdVector& vars);
  void selectMatchingVarsRealNames(PackageId packageId, const VersionCond& ver, VarIdVector& vars);
  void selectMatchingVarsRealNames(PackageId packageId, VarIdVector& vars);
  void selectMatchingVarsWithProvides(const IdPkgRel& rel, VarIdVector& vars);
  void selectMatchingVarsWithProvides(PackageId packageId, VarIdVector& vars);
  void selectMatchingVarsWithProvides(PackageId packageId, const VersionCond& ver, VarIdVector& vars);
  void selectTheNewest(VarIdVector& vars);
  void selectTheNewestByProvide(VarIdVector& vars, PackageId provideEntry);
  PackageId strToPackageId(const std::string& name);
  void whatConflictsAmongInstalled(VarId varId, VarIdVector& res, IdPkgRelVector& resRels);
  void whatDependsAmongInstalled(VarId varId, VarIdVector& res, IdPkgRelVector& resRels);
  void whatSatisfiesAmongInstalled(const IdPkgRel& rel, VarIdVector& res);
}; //class AbstractPackageScope;

#endif //DEEPSOLVER_ABSTRACT_PACKAGE_SCOPE_H;
