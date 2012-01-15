

#include"depsolver.h"
#include"PackageScope.h"

static versionSatisfies(const VersionCond& cond, Epoch epoch, const std:;string& ver, const std::string& release)
{
  //FIXME:
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

void selectMatchingWithVersionVarsAmongProvides(PackageId packageId, const VersionCond& ver, VarIdVector& vars)
{
  //Considering only provides entries and only with version information;
}

bool PackageScope::isINstalled(PackageId packageId) const
{
  //Checking only by real name;
  return 0;
}

bool PackageScope::isInstallWithProvides(PackageId packageId)
{
  //Checking by real name and by all known provides of installed packages;
}


void PackageScope::selectTheNewest(VarIdVector& vars)
{
  //Processing only real version of provided packages;
}

void PackageScope::selectTheNewestByProvide(VarIdVector& vars, PackageId provideEntry)
{
  //Checking the version of the given provide entry;
  //FIXME:there cannot be entries without version information;
}

bool PackageScope::allProvidesHaveTheVersion(const VarIdVector& vars, PackageId provideEntry)
{
}


