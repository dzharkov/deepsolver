
#include"deepsolver.h"
#include"PackageScopeContentBuilder.h"

inline void writeSizeValue(std::ofstream& s, size_t value)
{
  s.write(&value, sizeof(size_t));
}

void PackageScopeContentBuilder::saveToFile(const std::string& fileName) const
{
  SizeVector stringOffsets;
  stringOffsets.resize(m_stringValues.size());
  size_t k = 0;
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    {
      stringOffsets[i] = k;
      assert(m_stringValues[i] != NULL);
      k += strlen(m_stringValues[i]);
    }
  std::ofstream s(PACKAGE_LIST_FILE_NAME);
  assert(f.is_open());//FIXME:error checking;
  //Saving numbers of records;
  writeSizeValue(s, m_stringValues.size());
  writeSizeValue(s, m_names.size());
  writeSizeValue(s, m_pkgInfoVector.size());
  writeSizeValue(s, m_pkgRelVector.size());
  writeSizeValue(s, m_provides.size());
}

void PackageScopeContentBuilder::addPkg(const PkgFile& pkgFile)
{
  //We are interested only in name, epoch, version, release, and all relations;
  PkgInfo pkg;
  pkg.pkgId = registerName(pkgFile.name);
  pkg.epoch = pkgFile.epoch;
  assert(!pkgFile.version.empty() && !pkgFile.release.empty());
  pkg.ver = new char[pkgFile.version.length() + 1];
  strcpy(pkg.ver, pkgFile.version.c_str());
  m_stringValues.push_back(pkg.ver);
  pkg.release = new char[pkgFile.release.length() + 1];
  strcpy(pkg.release, pkgFile.release.c_str());
  m_stringValues.push_back(pkg.release);
  pkg.buildTime = pkgFile.buildTime;
  processRels(pkgFile.requires, pkg.requiresPos, pkg.requiresCount);
  processRels(pkgFile.conflicts, pkg.conflictsPos, pkg.conflictsCount);
  processRels(pkgFile.provides, pkg.providesPos, pkg.providesCount);
  processRels(pkgFile.obsoletes, pkg.obsoletesPos, pkg.obsoletesCount);
    m_pkgInfoVector.push_back(pkg);
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

void PackageScopeContentBuilder::commit()
{
  std::sort(m_pkgInfoVector.begin(), m_pkgInfoVector.end());
  std::sort(m_provideMap.begin(), m_provideMap.end());
}

void PackageScopeContentBuilder::processRels(const NamedPkgRelVector& rels, size_t& pos, size_t& count)
{
  if (rels.empty())
    {
      pos = 0;
      count = 0;
      return;
    }
  pos = m_relInfoVector.size();
  count = rels.size();
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
	  info.ver = new char[rel.ver.length() + 1];
	  strcpy(info.ver, rel.ver.c_str());
	  m_stringValues.push_back(info.ver);
	}
            m_relInfoVector.push_back(info);
    }
}

PackageId PackageScopeContentBuilder::registerName(const std::string& name)
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

void PackageScopeContentBuilder::freestringValues()
{
  for(stringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    delete[] m_stringValues[i];
  m_string values.clear();
}

