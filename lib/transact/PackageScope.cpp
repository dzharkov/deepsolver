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

#define HAS_VERSION(x) ((x).ver != NULL)

/**\brief Selects all variables to try by the specified package ID
 *
 * Selects all variables where specified package name is the name of the
 * package itself or the name of the one of the provides entry.
 */
static void selectVarsToTry(const PackageScopeContent& content,
			    const ProvideMap& provideMap,
			    PackageId pkgId,
			    VarIdVector& toTry,
			    bool includeItself)
{
  toTry.clear();
  if (includeItself)
    {
      VarId fromPos, toPos;
      content.locateRange(pkgId, fromPos, toPos);
      assert(fromPos < content.pkgInfoVector.size() && toPos < content.pkgInfoVector.size());
      for(VarId i = fromPos;i < toPos;i++)
	toTry.push_back(i);
    }
  VarIdVector providers;
  provideMap.searchProviders(pkgId, providers);
  for(VarIdVector::size_type i = 0;i < providers.size();i++)
    toTry.push_back(providers[i]);
  //Maybe it is good idea to perform dublications cleaning here,, but it can take time;
}

void PackageScope::selectMatchingVarsNoProvides(PackageId packageId, VarIdVector& vars)
{
  //Here we must process only real package names, no provides are required;
  vars.clear();
  VarId fromPos, toPos;
  m_content.locateRange(packageId, fromPos, toPos);
  assert(fromPos < m_content.pkgInfoVector.size() && toPos < m_content.pkgInfoVector.size());
  for(VarId i = fromPos;i < toPos;i++)
    {
      assert(m_content.pkgInfoVector[i].pkgId == packageId);
      vars.push_back(i);
    }
}

void PackageScope::selectMatchingVarsNoProvides(PackageId packageId, const VersionCond& ver, VarIdVector& vars)
{
  //Here we must process only real package names, no provides are required;
  vars.clear();
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  VarId fromPos, toPos;
  m_content.locateRange(packageId, fromPos, toPos);
  assert(fromPos < pkgs.size() && toPos < pkgs.size());
  for(VarId i = fromPos;i < toPos;i++)
    {
      assert(pkgs[i].pkgId == packageId);
      if (versionSatisfies(ver, pkgs[i].epoch, pkgs[i].ver, pkgs[i].release))
	vars.push_back(i);
    }
}

void PackageScope::selectMatchingVarsWithProvides(PackageId pkgId, VarIdVector& vars)
{
  //Package names and all their provides must be considered;
  vars.clear();
  selectVarsToTry(m_content, m_provideMap, pkgId, vars, 1);//1 means include packageId itself;
}

void PackageScope::selectMatchingVarsWithProvides(PackageId packageId, const VersionCond& ver, VarIdVector& vars)
{
  //Considering all package and provide names only with version information;
  vars.clear();
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  const RelInfoVector& rels = m_content.relInfoVector;
  VarIdVector toTry;
  selectVarsToTry(m_content, m_provideMap, packageId, toTry, 1);//1 means include packageId itself;
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
	  if (!HAS_VERSION(rels[pos + j]))
	    continue;  
	  assert(rels[pos + j].type != VerNone);
	  if (rels[pos + j].pkgId == packageId && versionOverlap(ver, VersionCond(rels[pos + j].ver, rels[pos + j].type)))
	    break;
	}
      if (j < count)
	vars.push_back(toTry[i]);
    }
}

void PackageScope::selectMatchingVarsAmongProvides(PackageId pkgId, VarIdVector& vars)
{
  //Only provides must be considered here;
  vars.clear();
  selectVarsToTry(m_content, m_provideMap, pkgId, vars, 0);//0 means do not include packageId itself;
}

void PackageScope::selectMatchingVarsAmongProvides(PackageId packageId, const VersionCond& ver, VarIdVector& vars)
{
  //Considering only provides entries and only with version information;
  vars.clear();
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  const RelInfoVector& rels = m_content.relInfoVector;
  VarIdVector toTry;
  selectVarsToTry(m_content, m_provideMap, packageId, toTry, 0);//0 means do not include packageId itself;
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
	  if (!HAS_VERSION(rels[pos + j]))
	    continue;  
	  assert(rels[pos + j].type != VerNone);
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
  const std::string maxVersion = constructFullVersion(currentMax);
  size_t hasCount = 0;
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    if (versionEqual(constructFullVersion(vars[i]), maxVersion))
      vars[hasCount++] = vars[i];
  assert(hasCount > 0);
  vars.resize(hasCount);
}

void PackageScope::selectTheNewestByProvide(VarIdVector& vars, PackageId provideEntry)
{
  if (vars.size() < 2)
    return;
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  const RelInfoVector& rels = m_content.relInfoVector;
  StringVector versions;
  versions.resize(vars.size());
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    {
      assert(vars[i] < pkgs.size());
      const PackageScopeContent::PkgInfo& pkg = pkgs[vars[i]];
      const size_t pos = pkg.providesPos;
      const size_t count = pkg.providesCount;
      assert(pos > 0 && count > 0);//It means the package has any provides;
      size_t j;
      for(j = 0;j < count;j++)
	{
	  assert(pos + j < rels.size());
	  if (rels[pos + j].pkgId == provideEntry)
	    break;
	}
      assert(j < count);//The package contains needed provide entry;
      assert(HAS_VERSION(rels[pos + j]));
      assert(rels[pos + j].type == VerEquals);//It is very strict constraint based on ALT Linux policy, but it will be better if it is so;
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
      if (!HAS_VERSION(rels[pos + j]))
	return 0;
    }
  return 1;
}

bool PackageScope::canBeSatisfiedByInstalled(PackageId pkgId)
{
  assert(pkgId != BAD_PACKAGE_ID);
  VarIdVector vars;
  selectVarsToTry(m_content, m_provideMap, pkgId, vars, 1);//1 means to include package itself;
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  //Just checking if one of them is installd;
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    {
      assert(vars[i] < pkgs.size());
      if (pkgs[vars[i]].flags & PkgFlagInstalled)
	return 1;
    }
  return 0;
}

bool PackageScope ::canBeSatisfiedByInstalled(PackageId pkgId, const VersionCond& ver)
{
  assert(pkgId != BAD_PACKAGE_ID);
  VarIdVector vars;
  selectVarsToTry(m_content, m_provideMap, pkgId, vars, 1);//1 means to include package itself;
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  const RelInfoVector& rels = m_content.relInfoVector;
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    {
      assert(vars[i] < pkgs.size());
      const PkgInfo& pkg = pkgs[vars[i]];
      if (!(pkg.flags & PkgFlagInstalled))
	continue;
      if (pkg.pkgId == pkgId && versionSatisfies(ver, pkg.epoch, pkg.ver, pkg.release))
	return 1;
      const size_t pos = pkg.providesPos;
      const size_t count = pkg.providesCount;
      if (count == 0)//There are no provides entries;
	continue;
      size_t j;
      for(j = 0;j < count;j++)
	{
	  assert(pos + j < rels.size());
	  if (rels[pos + j].pkgId == pkgId)
	    break;
	}
      if (j >= count)//No corresponding provide entry;
	continue;
      if (!HAS_VERSION(rels[pos + j]))//Provide entry has no version;
	continue;
      assert(rels[pos + j].type != VerNone);

      if (versionOverlap(ver, VersionCond(rels[pos + j].ver, rels[pos + j].type)))
	return 1;
    }
  return 0;
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

void PackageScope::whatDependsAmongINstalled(VarId varId, VarIdVector& res, IdPkgRelVector& resRels)
{
  res.clear();
  resRels.clear();
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  const RelInfoVector& rels = m_content.relInfoVector;
  assert(varId < pkgs.size());
  const PkgInfo& pkg = pkgs[varId];
  VarIdVector v;
  //First of all checking the package itself;
  m_requiresReferences.searchReferencesTo(pkgs[varId].pkgId, v);
  for(VarIdVector::size_type i = 0;i < v.size();i++)
    {
      assert(v[i] < pkgs.size());
      assert(pkgs[v[i]].flags & PkgFlagInstalled);
      PackageIdVector withoutVersion, withVersion;
      VersionCondVector versions;
      getRequires(v[i], withoutVersion, withVersion, versions);
      assert(withVersion.size() == versions.size());
      for(PackageIdVector::size_type k = 0;k < withoutVersion.size();k++)
	if (withoutVersion[k] == pkg.pkgId)
	  {
	    res.push_back(v[i]);
	    resRels.push_back(IdPkgRel(withoutVersion[k]));
	  }
      for(PackageIdVector::size_type k = 0;k < withVersion.size();k++)
	if (withVersion[k] == pkg.pkgId && versionSatisfies(versions[k], pkg.epoch, pkg.ver, pkg.release))
	  {
	    res.push_back(v[i]);
	    resRels.push_back(IdPkgRel(withVersion[k], versions[k]));
	  }
    } //For every package depending on varId without provides;
  //Now we check all provide entries of pkg;
  const size_t pos = pkg.providesPos;
  const size_t count = pkg.providesCount;
  for(RelInfoVector::size_type i = 0;i < count;i++)
    {
      assert(pos + i < rels.size());
      const RelInfo& rel = rels[pos + i];
      v.clear();
      m_requiresReferences.searchReferencesTo(rel.pkgId, v);
      for(VarIdVector::size_type k = 0;k < v.size();k++)
	{
	  assert(v[k] < pkgs.size());
	  assert(pkgs[v[k]].flags & PkgFlagInstalled);
	  PackageIdVector withoutVersion, withVersion;
	  VersionCondVector versions;
	  getRequires(v[k], withoutVersion, withVersion, versions);
	  assert(withVersion.size() == versions.size());
	  //Checking without version anyway;
	  for(PackageIdVector::size_type q = 0;q < withoutVersion.size();q++)
	    if (withoutVersion[q] == rel.pkgId)
	      {
		res.push_back(v[k]);
		resRels.push_back(IdPkgRel(withoutVersion[q]));
	      }
	  //With version must be checked only if provide entry has the version
	  if (HAS_VERSION(rel))
	    {
	      assert(rel.type == VerEquals);//FIXME:Actually it shouldn't be an assert, we can silently skip this provide;
	      for(PackageIdVector::size_type q = 0;q < withVersion.size();q++)
		if (withVersion[q] == rel.pkgId && versionSatisfies(versions[q], rel.ver))
		  {
		    res.push_back(v[k]);
		    resRels.push_back(IdPkgRel(withVersion[q], versions[q]));
		  }
	    }//if has version;
	}
    } //for provides;
}

void PackageScope::whatConflictsAmongINstalled(VarId varId, VarIdVector& res, IdPkgRelVector& resRels)
{
  res.clear();
  resRels.clear();
  const PkgInfoVector& pkgs = m_content.pkgInfoVector;
  const RelInfoVector& rels = m_content.relInfoVector;
  assert(varId < pkgs.size());
  const PkgInfo& pkg = pkgs[varId];
  VarIdVector v;
  //First of all checking the package itself;
  m_conflictsReferences.searchReferencesTo(pkgs[varId].pkgId, v);
  for(VarIdVector::size_type i = 0;i < v.size();i++)
    {
      assert(v[i] < pkgs.size());
      assert(pkgs[v[i]].flags & PkgFlagInstalled);
      PackageIdVector withoutVersion, withVersion;
      VersionCondVector versions;
      getConflicts(v[i], withoutVersion, withVersion, versions);
      assert(withVersion.size() == versions.size());
      for(PackageIdVector::size_type k = 0;k < withoutVersion.size();k++)
	if (withoutVersion[k] == pkg.pkgId)
	  {
	    res.push_back(v[i]);
	    resRels.push_back(IdPkgRel(withoutVersion[k]));
	  }
      for(PackageIdVector::size_type k = 0;k < withVersion.size();k++)
	if (withVersion[k] == pkg.pkgId && versionSatisfies(versions[k], pkg.epoch, pkg.ver, pkg.release))
	  {
	    res.push_back(v[i]);
	    resRels.push_back(IdPkgRel(withVersion[k], versions[k]));
	  }
    } //For every package depending on varId without provides;
  //Now we check all provide entries of pkg;
  const size_t pos = pkg.providesPos;
  const size_t count = pkg.providesCount;
  for(RelInfoVector::size_type i = 0;i < count;i++)
    {
      assert(pos + i < rels.size());
      const RelInfo& rel = rels[pos + i];
      v.clear();
      m_conflictsReferences.searchReferencesTo(rel.pkgId, v);
      for(VarIdVector::size_type k = 0;k < v.size();k++)
	{
	  assert(v[k] < pkgs.size());
	  assert(pkgs[v[k]].flags & PkgFlagInstalled);
	  PackageIdVector withoutVersion, withVersion;
	  VersionCondVector versions;
	  getConflicts(v[k], withoutVersion, withVersion, versions);
	  assert(withVersion.size() == versions.size());
	  //Checking without version anyway;
	  for(PackageIdVector::size_type q = 0;q < withoutVersion.size();q++)
	    if (withoutVersion[q] == rel.pkgId)
	      {
		res.push_back(v[k]);
		resRels.push_back(IdPkgRel(withoutVersion[q]));
	      }
	  //With version must be checked only if provide entry has the version
	  if (HAS_VERSION(rel))
	    {
	      assert(rel.type == VerEquals);//FIXME:Actually it shouldn't be an assert, we can silently skip this provide;
	      for(PackageIdVector::size_type q = 0;q < withVersion.size();q++)
		if (withVersion[q] == rel.pkgId && versionSatisfies(versions[q], rel.ver))
		  {
		    res.push_back(v[k]);
		    resRels.push_back(IdPkgRel(withVersion[q], versions[q]));
		  }
	    }//if has version;
	}
    } //for provides;
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

