
#include"deepsolver.h"
#include"PackageScope.h"
#include"version.h"

static void locateRange(const PackageScopeContent::PkgInfoVector& pkgs, PackageId packageId, VarId& fromPos, VarId& toPos )
{
  if (pkgs.empty())
    {
      fromPos = 0;
      toPos = 0;
      return;
    }
  //  std::cout << "called for " << packageId << std::endl;
  VarId l = 0, r = pkgs.size();
  while(l + 1 < r)
    {
      const VarId center = (l + r) / 2;
      assert(center < pkgs.size());
      //      std::cout << l << " " << center << " " << r << std::endl;
      //      std::cout << "+" << pkgs[l].pkgId << " " << pkgs[center].pkgId << " " << pkgs[r].pkgId << std::endl;
      if (pkgs[center].pkgId == packageId)
	{
	  //	  std::cout << "found" << std::endl;
	  fromPos = center;
	  toPos = center;
	  while(fromPos > 0 && pkgs[fromPos].pkgId == packageId)//VarId is unsigned, so all known overflow troubles is possible here, be careful!
	    fromPos--;
	  assert(fromPos < pkgs.size());
	  if (pkgs[fromPos].pkgId != packageId)
	    fromPos++;
	  assert(pkgs[fromPos].pkgId == packageId);
	  while(toPos < pkgs.size() && pkgs[toPos].pkgId == packageId)
	    toPos++;
	  assert(fromPos < toPos);
	  return;
	}
      if (pkgs[center].pkgId > packageId)
	r = center; else
	l = center;
    }
  assert(l <= r);
  const VarId center = (l + r) / 2;
  assert(center < pkgs.size());
  if (pkgs[center].pkgId == packageId)
    {
      //	  std::cout << "found after" << std::endl;
      fromPos = center;
      toPos = center;
      while(fromPos > 0 && pkgs[fromPos].pkgId == packageId)//VarId is unsigned, so all known overflow troubles is possible here, be careful!
	fromPos--;
      assert(fromPos < pkgs.size());
      if (pkgs[fromPos].pkgId != packageId)
	fromPos++;
      assert(pkgs[fromPos].pkgId == packageId);
      while(toPos < pkgs.size() && pkgs[toPos].pkgId == packageId)
	toPos++;
      assert(fromPos < toPos);
      return;
    }
  //We cannot find anything here;
  fromPos = 0;
  toPos = 0;
  //	  std::cout << "not found" << std::endl;
}

static void selectVarsToTry(const PackageScopeContent& content, PackageId packageId, VarIdVector& toTry, bool includeItself)
{
  //  std::cout << "selectVarsToTry()" << std::endl;
  PackageIdVector providers;
  content.getProviders(packageId, providers);
  if (includeItself)
    toTry.clear();
  for(PackageIdVector::size_type i = 0;i < providers.size();i++)
    {
      VarId fromPos, toPos;
      locateRange(content.getPkgs(), providers[i], fromPos, toPos);
      if (fromPos == toPos)
	continue;
      for(VarId k = fromPos;k < toPos;k++)
	toTry.push_back(k);
    }
  //  std::cout << "return" << std::endl;
}

PackageId PackageScope::packageIdOfVarId(VarId varId) const
{
  const PackageScopeContent::PkgInfoVector& pkgs = m_content.getPkgs();
  assert(varId < pkgs.size());
  return pkgs[varId].pkgId;
}

std::string PackageScope::constructPackageName(VarId varId) const
{
  const PackageScopeContent::PkgInfoVector& pkgs = m_content.getPkgs();
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

VarId PackageScope::registerTemporarily(const std::string& url)
{
  assert(0);
  //FIXME:not needed right now;
  return BAD_VAR_ID;
}

void PackageScope::selectMatchingVars(PackageId packageId, VarIdVector& vars)
{
  //Here we must process only real package names, no provides are required;
  vars.clear();
  const PackageScopeContent::PkgInfoVector& pkgs = m_content.getPkgs();
  VarId fromPos, toPos;
  locateRange(pkgs, packageId, fromPos, toPos);
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
  const PackageScopeContent::PkgInfoVector& pkgs = m_content.getPkgs();
  VarId fromPos, toPos;
  locateRange(pkgs, packageId, fromPos, toPos);
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
  const PackageScopeContent::PkgInfoVector& pkgs = m_content.getPkgs();
  const PackageScopeContent::RelInfoVector& rels = m_content.getRels();
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
  const PackageScopeContent::PkgInfoVector& pkgs = m_content.getPkgs();
  const PackageScopeContent::RelInfoVector& rels = m_content.getRels();
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

bool PackageScope::isINstalled(PackageId packageId) const
{
  //Checking only by real name;
  assert(0);//FIXME:currently not needed;
  return 0;
}

bool PackageScope::isInstallWithProvides(PackageId packageId)
{
  //Checking by real name and by all known provides of installed packages;
  assert(0);//FIXME:currently not needed;
  return 0;
}

void PackageScope::selectTheNewest(VarIdVector& vars)
{
  //Processing only real version of provided packages;
  if (vars.size() < 2)
    return;
  const PackageScopeContent::PkgInfoVector& pkgs = m_content.getPkgs();
  VarId currentMax = vars[0];
  assert(currentMax < pkgs.size());
  assert(pkgs[currentMax].ver != NULL);
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    {
      assert(vars[i] < pkgs.size());
  assert(pkgs[vars[i]].ver != NULL);
  if (versionGreater(pkgs[vars[i]].ver, pkgs[currentMax].ver))
    currentMax = i;
    }
  assert(currentMax < vars.size());
  const std::string maxVersion = pkgs[currentMax].ver;
  size_t hasCount = 0;
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    if (versionEqual(pkgs[vars[i]].ver, maxVersion))
      vars[hasCount++] = vars[i];
  assert(hasCount > 0);
  vars.resize(hasCount);
}

void PackageScope::selectTheNewestByProvide(VarIdVector& vars, PackageId provideEntry)
{
  //Checking the version of the given provide entry;
  if (vars.size() < 2)
    return;
  const PackageScopeContent::PkgInfoVector& pkgs = m_content.getPkgs();
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
      const PackageScopeContent::RelInfoVector& rels = m_content.getRels();
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
  const PackageScopeContent::PkgInfoVector& pkgs = m_content.getPkgs();
      const PackageScopeContent::RelInfoVector& rels = m_content.getRels();
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
  const PackageScopeContent::PkgInfoVector& pkgs = m_content.getPkgs();
  assert(varId < pkgs.size());
  const PackageScopeContent::PkgInfo& pkg = pkgs[varId];
  const PackageScopeContent::RelInfoVector& rels = m_content.getRels();
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
  const PackageScopeContent::PkgInfoVector& pkgs = m_content.getPkgs();
  assert(varId < pkgs.size());
  const PackageScopeContent::PkgInfo& pkg = pkgs[varId];
  const PackageScopeContent::RelInfoVector& rels = m_content.getRels();
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
