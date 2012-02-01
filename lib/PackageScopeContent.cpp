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

void PackageScopeContent::getProviders(PackageId provideId, PackageIdVector& providers) const
{
  providers.clear();
  if (m_provideMap.empty())
    return;
  ProvideMapItemVector::size_type l = 0, r = m_provideMap.size();
  while(l + 1 < r)
    {
      const ProvideMapItemVector::size_type center = (l + r) / 2;
      assert(center < m_provideMap.size());
      if (m_provideMap[center].provideId == provideId)
	{
	  ProvideMapItemVector::size_type fromPos, toPos;
	  fromPos = center;
	  toPos = center;
	  while(fromPos > 0 && m_provideMap[fromPos].provideId == provideId)//size_type is unsigned, so all known overflow troubles is possible here, be careful!
	    fromPos--;
	  assert(fromPos < m_provideMap.size());
	  if (m_provideMap[fromPos].provideId != provideId)
	    fromPos++;
	  assert(m_provideMap[fromPos].provideId == provideId);
	  while(toPos < m_provideMap.size() && m_provideMap[toPos].provideId == provideId)
	    toPos++;
	  assert(fromPos < toPos);
	  for(ProvideMapItemVector::size_type i = fromPos; i < toPos;i++)
	    providers.push_back(m_provideMap[i].pkgId);
	  return;
	}
      if (m_provideMap[center].provideId > provideId)
	r = center; else
	l = center;
    }
  assert(l <= r);
  const ProvideMapItemVector::size_type center = (l + r) / 2;
  assert(center < m_provideMap.size());
  if (m_provideMap[center].provideId == provideId)
    {
      ProvideMapItemVector::size_type fromPos, toPos;
      fromPos = center;
      toPos = center;
      while(fromPos > 0 && m_provideMap[fromPos].provideId == provideId)//size_type is unsigned, so all known overflow troubles is possible here, be careful!
	fromPos--;
      assert(fromPos < m_provideMap.size());
      if (m_provideMap[fromPos].provideId != provideId)
	fromPos++;
      assert(m_provideMap[fromPos].provideId == provideId);
      while(toPos < m_provideMap.size() && m_provideMap[toPos].provideId == provideId)
	toPos++;
      assert(fromPos < toPos);
      for(ProvideMapItemVector::size_type i = fromPos; i < toPos;i++)
	providers.push_back(m_provideMap[i].pkgId);
      return;
    }
  //We cannot find anything here;
  assert(providers.empty());
}

bool PackageScopeContent::checkName(const std::string& name) const
{
  assert(!name.empty());
  return m_namesToId.find(name) != m_namesToId.end();
}

PackageId PackageScopeContent::strToPackageId(const std::string& name) const
{
  assert(!name.empty());
  NameToPackageIdMap::const_iterator it = m_namesToId.find(name);
  assert(it != m_namesToId.end());
  return it->second;
}

std::string PackageScopeContent::packageIdToStr(PackageId packageId) const
{
  assert(packageId < m_names.size());
  return m_names[packageId];
}

const PackageScopeContent::PkgInfoVector& PackageScopeContent::getPkgs() const
{
  return m_pkgInfoVector;
}

const PackageScopeContent::RelInfoVector& PackageScopeContent::getRels() const
{
  return m_relInfoVector;
}
