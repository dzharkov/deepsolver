
#include"deepsolver.h"
#include"PackageScopeContentBuilder.h"

void PackageScopeContent::addPkg(const PkgFile& pkgFile)
{
  //We are interested only in name, epoch, version, release, and all relations;
  PkgInfo pkg;
  pkg.pkgId = registerName(pkgFile.name);
  pkg.epoch = pkgFile.epoch;
  assert(!pkgFile.version.empty() && !pkgFile.release.empty());
  pkg.ver = new char[pkgFile.version.length() + 1];//FIXME:this string must be registered to be automatically deallocated;
  strcpy(pkg.ver, pkgFile.version.c_str());
  pkg.release = new char[pkgFile.release.length() + 1];//FIXME:this string must be registered to be automatically deallocated;
  strcpy(pkg.release, pkgFile.release.c_str());
  pkg.buildTime = pkgFile.buildTime;
  processRels(pkgFile.requires, pkg.requiresPos, pkg.requiresCount);
  processRels(pkgFile.conflicts, pkg.conflictsPos, pkg.conflictsCount);
  processRels(pkgFile.provides, pkg.providesPos, pkg.providesCount);
  processRels(pkgFile.obsoletes, pkg.obsoletesPos, pkg.obsoletesCount);
    m_pkgInfoVector.push_back(pkg);
    //    std::cout << m_names.size() << std::endl;
}

void PackageScopeContent::addProvideMapItem(const std::string& provideName, const std::string& packageName)
{
  //  std::cout << provideName << "->" << packageName << std::endl;
  NameToPackageIdMap::const_iterator it = m_namesToId.find(provideName);
  assert(it != m_namesToId.end());//FIXME:must be an exception;
  const PackageId provideId = it->second;
  it = m_namesToId.find(packageName);
  assert(it != m_namesToId.end());//FIXME:must be an exception;
  const PackageId packageId = it->second;
  m_provideMap.push_back(ProvideMapItem(provideId, packageId));
}

void PackageScopeContent::commit()
{
  std::sort(m_pkgInfoVector.begin(), m_pkgInfoVector.end());
  std::sort(m_provideMap.begin(), m_provideMap.end());
}

void PackageScopeContent::processRels(const NamedPkgRelVector& rels, size_t& pos, size_t& count)
{
  if (rels.empty())
    {
      pos = 0;
      count = 0;
      return;
    }
  pos = m_relInfoVector.size();
  count = rels.size();
  //  std::cout << pos << ", " << count << std::endl;
  for(NamedPkgRelVector::size_type i = 0;i < count;i++)
    {
      const NamedPkgRel& rel = rels[i];
      RelInfo info;
      assert(!rel.pkgName.empty());
      info.pkgId = registerName(rel.pkgName);
      if (!rel.ver.empty())
	{
	  assert(rel.type != 0);
	  info.type = rel.type;
	  info.ver = new char[rel.ver.length() + 1];//FIXME:registration for automatic deallocationi;
	  strcpy(info.ver, rel.ver.c_str());
	}
            m_relInfoVector.push_back(info);
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
  m_namesToId.insert(NameToPackageIdMap::value_type(name, packageId));
  return packageId;
}

