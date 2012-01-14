

#include"depsolver.h"
#include"PackageScope.h"

PackageId PackageScope::strToPackageId(const std::string& name) const
{
  //FIXME:
}

std::string PackageScope::packageIdToStr(PackageId packageId) const
{
  //FIXME:
}


VarId PackageScope::registerTemporarily(const std::string& url)
{
  //FIXME:
}

void PackageScope::selectMatchingVars(PackageId packageId, VarIdVector& vars)
{
  //Here we must process only real package names, no provides are required;
}

void PackageScope::selectMatchingWithVersionVars(PackageId packageId, const VersionCond& ver, VarIdVector& vars)
{
  //Here we must process only real package names, no provides are required;
}

void PackageScope::selectMatchingVarsWithProvides(PackageId packageId, VarIdVector& vars)
{
  //Package names and all their provides must be considered;
}

void PackageScope::selectMatchingVarsAmongProvides(PackageId packageId, VarIdVector& vars)
{
  //Only provides must be considered here;
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


