
#include"deepsolver.h"
#include"PackageScopeContentBuilder.h"

inline void writeSizeValue(std::ofstream& s, size_t value)
{
  s.write((char*)&value, sizeof(size_t));
}

inline void writeUnsignedShortValue(std::ofstream& s, unsigned short value)
{
  s.write((char*)&value, sizeof(unsigned short));
}

inline void writeCharValue(std::ofstream& s, char value)
{
  s.write(&value, sizeof(char));
}

inline void writeStringValue(std::ofstream& s, const char* value)
{
  assert(value != NULL);
  s.write(value, strlen(value) + 1);
}

void PackageScopeContentBuilder::saveToFile(const std::string& fileName) const
{
  assert(!fileName.c_str());
  SizeVector stringOffsets;
  stringOffsets.resize(m_stringValues.size());
  size_t k = 0;
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    {
      stringOffsets[i] = k;
      assert(m_stringValues[i] != NULL);
      k += (strlen(m_stringValues[i]) + 1);
    }
  std::ofstream s(fileName.c_str());
  assert(s.is_open());//FIXME:error checking;
  //Saving numbers of records;
  assert(m_stringValues.size() == stringOffsets.size());
  writeSizeValue(s, m_stringValues.size());
  writeSizeValue(s, k);//we must have the total length of all strings;
  writeSizeValue(s, m_names.size());
  size_t totalNamesLen = 0;
  for(stringVector::size_type i = 0;i < m_names.size();i++)
    totalNamesLen += (m_names[i].length() + 1);
  writeSizeValue(s, totalNamesLen);
  writeSizeValue(s, m_pkgInfoVector.size());
  writeSizeValue(s, m_relInfoVector.size());
  writeSizeValue(s, m_provideMap.size());
  //Saving strings bounds;
  for(SizeVector::size_type i = 0;i < stringOffsets.size();i++)
    writeSizeValue(s, stringOffsets[i]);
  //Saving all version and release strings;
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    writeStringValue(s, m_stringValues[i]);
  //Saving names of packages and provides;
  for(StringVector::size_type i = 0;i < m_names.size();i++)
    writeStringValue(s, m_names[i].c_str());
  //Saving package list;
  for(PkgInfoVector::size_type i = 0;i < m_pkgInfoVector.size();i++)
    {
      const PkgInfo& info = m_pkgInfoVector[i];
      assert(i < m_pkgVerIndices.size());
      const size_t verId = m_pkgVerIndices[i];
      const size_t releaseId = verId + 1;
      writeSizeValue(s, info.pkgId);
      writeUnsignedShortValue(s, info.epoch);
      writeSizeValue(s, verId);
      writeSizeValue(s, releaseId);
      writeSizeValue(s, info.buildTime);
      writeSizeValue(s, info.requiresPos);
      writeSizeValue(s, info.requiresCount);
      writeSizeValue(s, info.providesPos);
      writeSizeValue(s, info.providesCount);
      writeSizeValue(s, info.conflictsPos);
      writeSizeValue(s, info.conflictsCount);
      writeSizeValue(s, info.obsoletesPos);
      writeSizeValue(s, info.obsoletesCount);
    }
  //Saving package relations;
  for(RelInfoVector::size_type i = 0;i < m_relInfoVector.size();i++)
    {
      const RelInfo& info = m_relInfoVector[i];
      assert(i < m_relVerIndices.size());
      const size_t verId = m_relVerIndices[i];
      writeSizeValue(s, info.pkgId);
      writeCharValue(s, info.type);
      writeSizeValue(s, verId);
    }
  //Saving provide map;
  for(ProvideMapItemVector::size_type i = 0;i < m_provideMap.size();i++)
    {
      writeSizeValue(s, m_provideMap[i].provideId);
      writeSizeValue(s, m_provideMap[i].pkgId);
    }
  s.flush();
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
  m_pkgVerIndices.push_back(m_stringValues.size() - 1);
  pkg.release = new char[pkgFile.release.length() + 1];
  strcpy(pkg.release, pkgFile.release.c_str());
  m_stringValues.push_back(pkg.release);
  //No need to save index of release string in m_stringValues vector since it always has the value of version string index +1;
  pkg.buildTime = pkgFile.buildTime;
  processRels(pkgFile.requires, pkg.requiresPos, pkg.requiresCount);
  processRels(pkgFile.conflicts, pkg.conflictsPos, pkg.conflictsCount);
  processRels(pkgFile.provides, pkg.providesPos, pkg.providesCount);
  processRels(pkgFile.obsoletes, pkg.obsoletesPos, pkg.obsoletesCount);
    m_pkgInfoVector.push_back(pkg);
}

void PackageScopeContentBuilder::addProvideMapItem(const std::string& provideName, const std::string& packageName)
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
	  m_relVerIndices.push_back(m_stringValues.size() - 1);
	} else 
	m_relVerIndices.push_back((size_t)-1);
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

void PackageScopeContentBuilder::freeStringValues()
{
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    delete[] m_stringValues[i];
  m_stringValues.clear();
}

