/*
 * The code of this file implies adding in future the special class with
 * automatic string freeing implementation. There are a lot of
 * dynamically allocated string values without using of std::string due
 * to memory-efficiency reasons. All string values must be automatically
 * deleted on PackageScopeContent destruction to prevent memory leaks in
 * case of unexpected exceptions.
 */

#include"depsolver.h"
#include"PackageScopeContent.h"

void PackageScopeContent::add(const PkgFile& pkgFile)
{
  //We are interested only in name, epoch, version, release, and all relations;
  PkgINfo pkg;
  pkg.pkgId = registerName(pkgFile.name);
  pkg.epoch = pkgFile.epoch;
  assert(!pkgFile.version.empty() && !pkgFile.release.empty());
  pkgFile.ver = new char[pkgFile.ver.length() + 1];//FIXME:this string must be registered to be automatically deallocated;
  strcpy(pkg.ver, pkgFile.ver.c_str());
  pkgFile.release = new char[pkgFile.release.length() + 1];//FIXME:this string must be registered to be automatically deallocated;
  strcpy(pkg.release, pkgFile.release.c_str());
  pkg.buildTime = pkgFile.buildTime;
  processRels(pkgFile.requires, pkg.requiresPos, pkg.requiresCount);
  processRels(pkgFile.conflicts, pkg.conflictsPos, pkg.conflictsCount);
  processRels(pkgFile.provides, pkg.providesPos, pkg.providesCount);
  processRels(pkgFile.obsoletes, pkg.obsoletesPos, pkg.obsoletesCount);
  m_pkgINfoList.push_back(pkg);
}

void PackageScopeContent::commit()
{
  m_pkgInfoVector.reserve(m_pkgInfoList.size());
  for(PkgInfoList::const_iterator it = m_pkgInfoList.begin(); it != m_pkgInfoList.end();it++)
    m_pkgInfoVector.push_back(*it);
  m_pkgInfoList.clear();
  m_relInfoVector.reserve(m_relInfoList.size());
  for(RelInfoList::const_iterator it = m_relInfoList.begin();it != m_relInfoList.end();it++)
    m_relInfoVector.push_back(*it);
  m_relInfoList.clear();
}

void PackageScopeContent::processRels(const NamedPkgRelVector^& rels, size_t& pos, size_t& count)
{
  if (rels.empty())
    {
      pos = 0;
      count = 0;
      return;
    }
  pos = m_pkgRelList.size();
  count = rels.size();
  for(NamedPkgRelVector::size_type i = 0;i < rels.size();i++)
    {
      RelInfo info;
      assert(rels[i].pkgName.empty());
      info.pkgId = registerName(rels[i].pkgName);
      if (!rels[i].ver.empty())
	{
	  assert(rels[i].type != 0);
	  info.type = rels[i].type;
	  info.ver = new char[rels[i].ver.length() + 1];//FIXME:registration for automatic deallocationi;
	  strcpy(info.ver, rels[i].ver.c_str());
	}
      m_relInfoList.push_back(info);
    }
}

PackageId PackageScopeContent::registerName(const std::string& name)
{
  assert(!name.empty());
  NameToPackageIdMap::const_iterator it = m_namesToId.find(name);
  if (it != m_namesToId.end())
    return it->second;
  m_names.push_back(name);
  const PackageId packageId = m_names.size() - 1;
  m_namesToId.push_back(NameToPackageIdMap::value_type(name, packageId));
  return packageId;
}
