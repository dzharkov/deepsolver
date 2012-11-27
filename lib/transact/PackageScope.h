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

#include"AbstractPackageScope.h"
#include"AbstractPackageBackEnd.h"
#include"PackageScopeContent.h"
#include"ProvideMap.h"
#include"InstalledReferences.h"

class PackageScope: public AbstractPackageScope
{
public:
  PackageScope(const AbstractPackageBackEnd& backEnd,
	       const PackageScopeContent& content,
	       const ProvideMap& provideMap,
	       const InstalledReferences& installedRequiresEntries,
	       const InstalledReferences& installedConflictsEntries)
    : m_backEnd(backEnd),
      m_content(content), 
      m_provideMap(provideMap),
      m_installedRequiresEntries(installedRequiresEntries),
      m_installedConflictsEntries(installedConflictsEntries) {}

  virtual ~PackageScope() {}

public:
  PackageId packageIdOfVarId(VarId varId) const;
  std::string constructPackageName(VarId varId) const;
  std::string constructPackageNameWithBuildTime(VarId varId) const;
  bool checkName(const std::string& name) const;
  PackageId strToPackageId(const std::string& name) const;
  std::string packageIdToStr(PackageId packageId) const;

  void selectMatchingVarsAmongProvides(const IdPkgRel& rel, VarIdVector& vars);
  void selectMatchingVarsAmongProvides(PackageId packageId, VarIdVector& vars);
  void selectMatchingVarsAmongProvides(PackageId packageId, const VersionCond& ver, VarIdVector& vars);
  void selectMatchingVarsRealNames(const IdPkgRel& rel, VarIdVector& vars);
  void selectMatchingVarsRealNames(PackageId packageId, VarIdVector& vars);
  void selectMatchingVarsRealNames(PackageId packageId, const VersionCond& ver, VarIdVector& vars);
  void selectMatchingVarsWithProvides(const IdPkgRel& rel, VarIdVector& vars);
  void selectMatchingVarsWithProvides(PackageId packageId, VarIdVector& vars);
  void selectMatchingVarsWithProvides(PackageId packageId, const VersionCond& ver, VarIdVector& vars);

  bool isInstalled(VarId varId) const;
  void selectTheNewest(VarIdVector& vars);
  void selectTheNewestByProvide(VarIdVector& vars, PackageId provideEntry);
  bool allProvidesHaveTheVersion(const VarIdVector& vars, PackageId provideEntry);

  void getRequires(VarId varId, IdPkgRelVector& res);
  void getConflicts(VarId varId, IdPkgRelVector& res);

  void whatConflictsAmongInstalled(VarId varId, VarIdVector& res, IdPkgRelVector& resRels);
  void whatDependsAmongInstalled(VarId varId, VarIdVector& res, IdPkgRelVector& resRels);
  void whatSatisfiesAmongInstalled(const IdPkgRel& rel, VarIdVector& res);

private:
  void getRequires(VarId varId,
		   PackageIdVector& depWithoutVersion,
		   PackageIdVector& depWithVersion,
		   VersionCondVector& versions) const;

  void getConflicts(VarId varId,
		    PackageIdVector& withoutVersion,
		    PackageIdVector& withVersion,
		    VersionCondVector& versions) const;

private:
  int versionCompare(const std::string& ver1, const std::string& ver2) const;
  bool versionOverlap(const VersionCond& ver1, const VersionCond& ver2) const;
  bool versionEqual(const std::string& ver1, const std::string& ver2) const;
bool versionGreater(const std::string& ver1, const std::string& ver2) const;
  std::string constructFullVersion(VarId varId) const;

private:
  typedef PackageScopeContent::PkgInfo PkgInfo;
  typedef PackageScopeContent::RelInfo RelInfo;
  typedef PackageScopeContent::PkgInfoVector PkgInfoVector;
  typedef PackageScopeContent::RelInfoVector RelInfoVector;

private:
  const AbstractPackageBackEnd& m_backEnd;
  const PackageScopeContent& m_content;
  const ProvideMap& m_provideMap;
  const InstalledReferences& m_installedRequiresEntries;
  const InstalledReferences& m_installedConflictsEntries;
}; //class PackageScope; 

#endif //DEEPSOLVER_PACKAGE_SCOPE_H;
