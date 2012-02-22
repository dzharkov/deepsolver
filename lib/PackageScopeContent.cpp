
#include"deepsolver.h"
#include"PackageScopeContent.h"

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
