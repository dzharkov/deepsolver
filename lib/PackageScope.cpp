

#include"depsolver.h"
#include"PackageScope.h"
#include"version.h"

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
  for(VarId i = 0;i < pkgs.size();i++)
    if (pkgs[i].pkgId == packageId)
      vars.push_back(i);
}

void PackageScope::selectMatchingWithVersionVars(PackageId packageId, const VersionCond& ver, VarIdVector& vars)
{
  //Here we must process only real package names, no provides are required;
  vars.clear();
  const PackageScopeContent::PkgInfoVector& pkgs = m_content.getPkgs();
  for(VarId i = 0;i < pkgs.size();i++)
    if (pkgs[i].pkgId == packageId && versionSatisfies(ver, pkgs[i].epoch, pkgs[i].ver, pkgs[i].release))
      vars.push_back(i);
}

void PackageScope::selectMatchingVarsWithProvides(PackageId packageId, VarIdVector& vars)
{
  //Package names and all their provides must be considered;
  vars.clear();
  const PackageScopeContent::PkgInfoVector& pkgs = m_content.getPkgs();
  for(VarId i = 0;i < pkgs.size();i++)
    {
      if (pkgs[i].pkgId == packageId)
	{
	  vars.push_back(i);
	  continue;
	}
      const size_t pos = pkgs[i].providesPos;
      const size_t count = pkgs[i].providesCount;
      if (pos == 0 || count == 0)//There are no provides entries;
	continue;
      const PackageScopeContent::RelInfoVector& rels = m_content.getRels();
      size_t j;
      for(j = 0;j < count;j++)
	{
	  assert(pos + j < rels.size());
	  if (rels[pos + j].pkgId == packageId)
	    break;
	}
      if (j < count)
	vars.push_back(i);
    }
}

void PackageScope::selectMatchingVarsAmongProvides(PackageId packageId, VarIdVector& vars)
{
  //Only provides must be considered here;
  vars.clear();
  const PackageScopeContent::PkgInfoVector& pkgs = m_content.getPkgs();
  for(VarId i = 0;i < pkgs.size();i++)
    {
      const size_t pos = pkgs[i].providesPos;
      const size_t count = pkgs[i].providesCount;
      if (pos == 0 || count == 0)//There are no provides entries;
	continue;
      const PackageScopeContent::RelInfoVector& rels = m_content.getRels();
      size_t j;
      for(j = 0;j < count;j++)
	{
	  assert(pos + j < rels.size());
	  if (rels[pos + j].pkgId == packageId)
	    break;
	}
      if (j < count)
	vars.push_back(i);
    }
}

void PackageScope::selectMatchingWithVersionVarsAmongProvides(PackageId packageId, const VersionCond& ver, VarIdVector& vars)
{
  //Considering only provides entries and only with version information;
  vars.clear();
  const PackageScopeContent::PkgInfoVector& pkgs = m_content.getPkgs();
  for(VarId i = 0;i < pkgs.size();i++)
    {
      const size_t pos = pkgs[i].providesPos;
      const size_t count = pkgs[i].providesCount;
      if (pos == 0 || count == 0)//There are no provides entries;
	continue;
      const PackageScopeContent::RelInfoVector& rels = m_content.getRels();
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
	vars.push_back(i);
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
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    {
      assert(vars[i] < pkgs.size());
      const PackageScopeContent::PkgInfo& pkg = pkgs[vars[i]];
      const size_t pos = pkg.providesPos;
      const size_t count = pkg.providesCount;
      assert(pos > 0 && count > 0);
      const PackageScopeContent::RelInfoVector& rels = m_content.getRels();
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
