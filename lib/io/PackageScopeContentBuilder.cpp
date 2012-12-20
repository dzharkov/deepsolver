/*
   Copyright 2011-2012 ALT Linux
   Copyright 2011-2012 Michael Pozhidaev

   This file is part of the Deepsolver.

   Deepsolver is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   Deepsolver is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
*/

#include"deepsolver.h"
#include"PackageScopeContentBuilder.h"
#include"OperationCore.h"

#define THROW_INTERNAL_ERROR throw OperationException(OperationException::InternalIOProblem)

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
  logMsg(LOG_DEBUG, "Starting saving prepared package scope content to binary file \'%s\'", fileName.c_str());
  assert(!fileName.empty());
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
  if (!s.is_open())
    {
      logMsg(LOG_ERR, "An error occurred while opening \'%s\' for writing", fileName.c_str());
      THROW_INTERNAL_ERROR;
    }
  assert(m_stringValues.size() == stringOffsets.size());
  //Saving string buffer length;
  size_t controlValue = 0;
  writeSizeValue(s, k);//we must have the total length of all strings;
  logMsg(LOG_DEBUG, "%zu bytes in all string constants with trailing zeroes", k);
  controlValue += k;
  writeSizeValue(s, m_names.size());
  logMsg(LOG_DEBUG, "%zu package names", m_names.size());
  controlValue += m_names.size();
  size_t totalNamesLen = 0;
  for(StringVector::size_type i = 0;i < m_names.size();i++)
    totalNamesLen += (m_names[i].length() + 1);
  writeSizeValue(s, totalNamesLen);
  logMsg(LOG_DEBUG, "%zu bytes in all package names with trailing zeroes", totalNamesLen);
  controlValue += totalNamesLen;
  writeSizeValue(s, m_pkgInfoVector.size());
  logMsg(LOG_DEBUG, "%zu packages", m_pkgInfoVector.size());
  controlValue += m_pkgInfoVector.size();
  writeSizeValue(s, m_relInfoVector.size());
  logMsg(LOG_DEBUG, "%zu package relations", m_relInfoVector.size());
  controlValue += m_relInfoVector.size();
  writeSizeValue(s, controlValue);
  logMsg(LOG_DEBUG, "Saved control value %zu", controlValue);
  if (m_pkgInfoVector.empty())
    {
      logMsg(LOG_DEBUG, "There are no packages, nothing to save");
      return;
    }
  //Saving all version and release strings;
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    writeStringValue(s, m_stringValues[i]);
  //Saving names of packages;
  for(StringVector::size_type i = 0;i < m_names.size();i++)
    writeStringValue(s, m_names[i].c_str());
  //Saving package list;
  for(PkgInfoVector::size_type i = 0;i < m_pkgInfoVector.size();i++)
    {
      const PkgInfo& info = m_pkgInfoVector[i];
      assert(m_stringValues[info.aux] == info.ver);
      assert(m_stringValues[info.aux + 1] == info.release);
      writeSizeValue(s, info.pkgId);
      writeUnsignedShortValue(s, info.epoch);
      writeSizeValue(s, stringOffsets[info.aux]);
      writeSizeValue(s, stringOffsets[info.aux+ 1]);
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
  assert(m_relVerIndices.size() == m_relInfoVector.size());
  for(RelInfoVector::size_type i = 0;i < m_relInfoVector.size();i++)
    {
      const RelInfo& info = m_relInfoVector[i];
      assert(m_relVerIndices[i] == (size_t)-1 || m_stringValues[m_relVerIndices[i]] == m_relInfoVector[i].ver);
      writeSizeValue(s, info.pkgId);
      writeCharValue(s, info.type);
      if (m_relVerIndices[i] != (size_t)-1)
	writeSizeValue(s, stringOffsets[m_relVerIndices[i]]); else
	writeSizeValue(s, (size_t)-1);
    }
  s.flush();
}

void PackageScopeContentBuilder::onNewPkgFile(const PkgFile& pkgFile)
{
  //We are interested only in name, epoch, version, release, and all relations;
  PkgInfo pkg;
  pkg.pkgId = registerName(pkgFile.name);
  pkg.epoch = pkgFile.epoch;
  assert(!pkgFile.version.empty() && !pkgFile.release.empty());
  pkg.ver = new char[pkgFile.version.length() + 1];
  strcpy(pkg.ver, pkgFile.version.c_str());
  m_stringValues.push_back(pkg.ver);
  pkg.aux = m_stringValues.size() - 1;
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

void PackageScopeContentBuilder::commit()
{
  //FIXME:Additional testing with small amount of data or without data at all;
  assert(m_relVerIndices.size() == m_relInfoVector.size());
  StringVector names(m_names);
  std::sort(names.begin(), names.end());
  SizeVector newPlaces;
  newPlaces.resize(m_names.size());
  for(StringVector::size_type i = 0;i < m_names.size();i++)
    {
      StringVector::size_type res = 0;
      StringVector::size_type l = 0, r = names.size();
      while(1)
	{
	  assert(l <= r);
	  const StringVector::size_type c = (l + r) / 2;
	  assert(c < names.size());
	  if (names[c] == m_names [i])
	    {
	      res = c;
	      break;
	    }
	  if (names[c] > m_names[i])
	    r = c; else
	    l = c;
	} //while(1);
      newPlaces[i] = res;
    }
  for(PkgInfoVector::size_type i = 0;i < m_pkgInfoVector.size();i++)
    {
      assert(m_pkgInfoVector[i].pkgId < newPlaces.size());
      m_pkgInfoVector[i].pkgId = newPlaces[m_pkgInfoVector[i].pkgId];
    }
  for(RelInfoVector::size_type i = 0;i < m_relInfoVector.size();i++)
    {
      assert(m_relInfoVector[i].pkgId < newPlaces.size());
      m_relInfoVector[i].pkgId = newPlaces[m_relInfoVector[i].pkgId];
    }
  m_names = names;
  std::sort(m_pkgInfoVector.begin(), m_pkgInfoVector.end());
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
