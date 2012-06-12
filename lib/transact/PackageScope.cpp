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

#include"deepsolver.h"
#include"PackageScope.h"
#include"version.h"


static void selectVarsToTry(const PackageScopeContent& content, PackageId packageId, VarIdVector& toTry, bool includeItself)
{
  PackageIdVector providers;
  content.getProviders(packageId, providers);
  if (includeItself)
    toTry.clear();
  for(PackageIdVector::size_type i = 0;i < providers.size();i++)
    {
      VarId fromPos, toPos;
      content.locateRange(providers[i], fromPos, toPos);
      if (fromPos == toPos)
	continue;
      for(VarId k = fromPos;k < toPos;k++)
	toTry.push_back(k);
    }
}

PackageId PackageScope::packageIdOfVarId(VarId varId) const
{
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  assert(varId < pkgs.size());
  return pkgs[varId].pkgId;
}

std::string PackageScope::constructPackageName(VarId varId) const
{
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  assert(varId < pkgs.size());
  const PackageScopeContent::PkgInfo& pkg = pkgs[varId];
  std::string res = packageIdToStr(pkg.pkgId);
  assert(pkg.ver != NULL);
  res += "-";
  res += pkg.ver;
  assert(pkg.release != NULL);
  res += "-";
  res += pkg.release;
  return res;
}

std::string PackageScope::constructFullVersion(VarId varId) const
{
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  assert(varId < pkgs.size());
  const PackageScopeContent::PkgInfo& pkg = pkgs[varId];
  assert(pkg.ver != NULL && pkg.release != NULL);
  std::ostringstream ss;
  if (pkg.epoch > 0)
    ss << pkg.epoch << ":";
  ss << pkg.ver << "-" << pkg.release;
  return ss.str();
}

bool PackageScope::checkName(const std::string& name) const
{
  return m_content.checkName(name);
}

PackageId PackageScope::strToPackageId(const std::string& name) const
{
  return m_content.strToPackageId(name);
}

std::string PackageScope::packageIdToStr(PackageId packageId) const
{
  return m_content.packageIdToStr(packageId);
}

void PackageScope::selectMatchingVars(PackageId packageId, VarIdVector& vars)
{
  //Here we must process only real package names, no provides are required;
  vars.clear();
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  VarId fromPos, toPos;
  m_content.locateRange(packageId, fromPos, toPos);
  assert(fromPos <= toPos);
  if (fromPos == toPos)
    return;
  assert(fromPos < pkgs.size() && toPos <= pkgs.size());
  assert(pkgs[fromPos].pkgId == packageId);
  for(VarId i = fromPos;i < toPos;i++)
    {
      assert(pkgs[i].pkgId == packageId);
      vars.push_back(i);
    }
}

void PackageScope::selectMatchingWithVersionVars(PackageId packageId, const VersionCond& ver, VarIdVector& vars)
{
  //Here we must process only real package names, no provides are required;
  vars.clear();
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  VarId fromPos, toPos;
  m_content.locateRange(packageId, fromPos, toPos);
  assert(fromPos <= toPos);
  if (fromPos == toPos)
    return;
  assert(fromPos < pkgs.size() && toPos <= pkgs.size());
  assert(pkgs[fromPos].pkgId == packageId);
  for(VarId i = fromPos;i < toPos;i++)
    {
      assert(pkgs[i].pkgId == packageId);
      if (versionSatisfies(ver, pkgs[i].epoch, pkgs[i].ver, pkgs[i].release))
	vars.push_back(i);
    }
}

void PackageScope::selectMatchingVarsWithProvides(PackageId packageId, VarIdVector& vars)
{
  //Package names and all their provides must be considered;
  vars.clear();
  selectVarsToTry(m_content, packageId, vars, 1);//1 means include packageId itself;
}

void PackageScope::selectMatchingWithVersionVarsWithProvides(PackageId packageId, const VersionCond& ver, VarIdVector& vars)
{
  //Considering all package and provide names only with version information;
  vars.clear();
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  const RelInfoVector& rels = m_content.relInfoVector;
  VarIdVector toTry;
  selectVarsToTry(m_content, packageId, toTry, 1);//1 means include packageId itself;
  for(VarIdVector::size_type i = 0;i < toTry.size();i++)
    {
      assert(toTry[i] < pkgs.size());
      if (pkgs[toTry[i]].pkgId == packageId && versionSatisfies(ver, pkgs[toTry[i]].epoch, pkgs[toTry[i]].ver, pkgs[toTry[i]].release))
	{
	  vars.push_back(toTry[i]);
	  continue;
	}
      const size_t pos = pkgs[toTry[i]].providesPos;
      const size_t count = pkgs[toTry[i]].providesCount;
      if (count == 0)//There are no provides entries;
	continue;
      size_t j;
      for(j = 0;j < count;j++)
	{
	  assert(pos + j < rels.size());
	  if (rels[pos + j].ver == NULL)
	    continue;  
	  assert(rels[pos + j].type != 0);
	  if (rels[pos + j].pkgId == packageId && versionOverlap(ver, VersionCond(rels[pos + j].ver, rels[pos + j].type)))
	    break;
	}
      if (j < count)
	vars.push_back(toTry[i]);
    }
}

void PackageScope::selectMatchingVarsAmongProvides(PackageId packageId, VarIdVector& vars)
{
  //Only provides must be considered here;
  vars.clear();
  selectVarsToTry(m_content, packageId, vars, 0);//0 means do not include packageId itself;
}

void PackageScope::selectMatchingWithVersionVarsAmongProvides(PackageId packageId, const VersionCond& ver, VarIdVector& vars)
{
  //Considering only provides entries and only with version information;
  vars.clear();
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  const RelInfoVector& rels = m_content.relInfoVector;
  VarIdVector toTry;
  selectVarsToTry(m_content, packageId, toTry, 0);//0 means do not include packageId itself;
  for(VarIdVector::size_type i = 0;i < toTry.size();i++)
    {
      assert(toTry[i] < pkgs.size());
      const size_t pos = pkgs[toTry[i]].providesPos;
      const size_t count = pkgs[toTry[i]].providesCount;
      if (count == 0)//There are no provides entries;
	continue;
      size_t j;
      for(j = 0;j < count;j++)
	{
	  assert(pos + j < rels.size());
	  if (rels[pos + j].ver == NULL)
	    continue;  
	  assert(rels[pos + j].type != 0);
	  if (rels[pos + j].pkgId == packageId && versionOverlap(ver, VersionCond(rels[pos + j].ver, rels[pos + j].type)))
	    break;
	}
      if (j < count)
	vars.push_back(toTry[i]);
    }
}

bool PackageScope::isInstalled(VarId varId) const
{
  assert(varId != BAD_VAR_ID);
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  assert(varId < pkgs.size());
  return pkgs[varId].flags & PkgFlagInstalled;
}

void PackageScope::selectInstalledNoProvides(PackageId pkgId, VarIdVector& vars) const
{
  vars.clear();
  assert(pkgId != BAD_PACKAGE_ID);
  VarId fromVarId, toVarId;
  m_content.locateRange(pkgId, fromVarId, toVarId);
  if (fromVarId == toVarId)
    return;
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  for(VarId i = fromVarId;i < toVarId;i++)
    if (pkgs[i].flags & PkgFlagInstalled)
      vars.push_back(i);
}

void PackageScope::selectInstalledWithVersionNoProvides(PackageId pkgId, const VersionCond& ver, VarIdVector& vars) const
{
  vars.clear();
  //FIXME:
  assert(0);
}

void PackageScope::selectInstallWithProvides(PackageId pkgId, VarIdVector& vars) const
{
  //Checking by real name and by all known provides of installed packages;
  assert(0);//FIXME:currently not needed;
}

void PackageScope::selectTheNewest(VarIdVector& vars)
{
  //Processing only real version of provided packages;
  if (vars.size() < 2)
    return;
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  VarId currentMax = vars[0];
  assert(currentMax < pkgs.size());
  assert(pkgs[currentMax].ver != NULL);
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    {
      assert(vars[i] < pkgs.size());
  assert(pkgs[vars[i]].ver != NULL);
  if (versionGreater(constructFullVersion(vars[i]), constructFullVersion(currentMax)))
    currentMax = vars[i];
    }
  assert(currentMax < pkgs.size());
  const std::string maxVersion = constructFullVersion(currentMax);;
  size_t hasCount = 0;
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    if (versionEqual(constructFullVersion(vars[i]), maxVersion))
      vars[hasCount++] = vars[i];
  assert(hasCount > 0);
  vars.resize(hasCount);
}

void PackageScope::selectTheNewestByProvide(VarIdVector& vars, PackageId provideEntry)
{
  //Checking the version of the given provide entry;
  if (vars.size() < 2)
    return;
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  StringVector versions;
  versions.resize(vars.size());
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    {
      assert(vars[i] < pkgs.size());
      const PackageScopeContent::PkgInfo& pkg = pkgs[vars[i]];
      const size_t pos = pkg.providesPos;
      const size_t count = pkg.providesCount;
      assert(pos > 0 && count > 0);
      size_t j;
      const RelInfoVector& rels = m_content.relInfoVector;
      for(j = 0;j < count;j++)
	{
	  assert(pos + j < rels.size());
	  if (rels[pos + j].pkgId == provideEntry)
	    break;
	}
      assert(j < count);
      assert(rels[pos + j].ver != NULL);
      versions[i] = rels[pos + j].ver;
    }
  assert(vars.size() == versions.size());
  size_t currentMax = 0;
  for(StringVector::size_type i = 0;i < versions.size();i++)
    if (versionGreater(versions[i], versions[currentMax]))
      currentMax = i;
  const std::string maxVersion = versions[currentMax];
  size_t hasCount = 0;
  for(StringVector::size_type i = 0;i < versions.size();i++)
    if (versionEqual(versions[i], maxVersion))
      vars[hasCount++] = vars[i];
  assert(hasCount > 0);
  vars.resize(hasCount);
}

bool PackageScope::allProvidesHaveTheVersion(const VarIdVector& vars, PackageId provideEntry)
{
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
      const RelInfoVector& rels = m_content.relInfoVector;
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    {
      assert(vars[i] < pkgs.size());
      const PackageScopeContent::PkgInfo& pkg = pkgs[vars[i]];
      const size_t pos = pkg.providesPos;
      const size_t count = pkg.providesCount;
      assert(count > 0);
      size_t j;
      for(j = 0;j < count;j++)
	{
	  assert(pos + j < rels.size());
	  if (rels[pos + j].pkgId == provideEntry)
	    break;
	}
      assert(j < count);
      if (rels[pos + j].ver == NULL)
	return 0;
    }
  return 1;
}

void PackageScope::getRequires(VarId varId, PackageIdVector& depWithoutVersion, PackageIdVector& depWithVersion, VersionCondVector& versions) const
{
  depWithoutVersion.clear();
  depWithVersion.clear();
  versions.clear();
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  assert(varId < pkgs.size());
  const PackageScopeContent::PkgInfo& pkg = pkgs[varId];
  const RelInfoVector& rels = m_content.relInfoVector;
  const size_t pos = pkg.requiresPos;
  const size_t count = pkg.requiresCount;
  for(size_t i = 0;i < count;i++)
    {
      assert(pos + i < rels.size());
      if (rels[pos + i].ver == NULL)
	depWithoutVersion.push_back(rels[pos + i].pkgId); else 
	{
	  depWithVersion.push_back(rels[pos + i].pkgId);
	  versions.push_back(VersionCond(rels[pos + i].ver, rels[pos + i].type));
	}
    }
}

void PackageScope::getConflicts(VarId varId, PackageIdVector& withoutVersion, PackageIdVector& withVersion, VersionCondVector& versions) const
{
  withoutVersion.clear();
  withVersion.clear();
  versions.clear();
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  assert(varId < pkgs.size());
  const PackageScopeContent::PkgInfo& pkg = pkgs[varId];
  const RelInfoVector& rels = m_content.relInfoVector;
  const size_t pos = pkg.conflictsPos;
  const size_t count = pkg.conflictsCount;
  for(size_t i = 0;i < count;i++)
    {
      assert(pos + i < rels.size());
      if (rels[pos + i].ver == NULL)
	withoutVersion.push_back(rels[pos + i].pkgId); else 
	{
	  withVersion.push_back(rels[pos + i].pkgId);
	  versions.push_back(VersionCond(rels[pos + i].ver, rels[pos + i].type));
	}
    }
}

