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
#include"PackageScopeContent.h"

void PackageScopeContent::locateRange(PackageId packageId, VarId& fromPos, VarId& toPos ) const
{
  if (pkgInfoVector.empty())
    {
      fromPos = 0;
      toPos = 0;
      return;
    }
  VarId l = 0, r = pkgInfoVector.size();
  while(l + 1 < r)
    {
      const VarId center = (l + r) / 2;
      assert(center < pkgInfoVector.size());
      if (pkgInfoVector[center].pkgId == packageId)
	{
	  fromPos = center;
	  toPos = center;
	  while(fromPos > 0 && pkgInfoVector[fromPos].pkgId == packageId)//VarId is unsigned, so all known overflow troubles is possible here, be careful!
	    fromPos--;
	  assert(fromPos < pkgInfoVector.size());
	  if (pkgInfoVector[fromPos].pkgId != packageId)
	    fromPos++;
	  assert(pkgInfoVector[fromPos].pkgId == packageId);
	  while(toPos < pkgInfoVector.size() && pkgInfoVector[toPos].pkgId == packageId)
	    toPos++;
	  assert(fromPos < toPos);
	  return;
	}
      if (pkgInfoVector[center].pkgId > packageId)
	r = center; else
	l = center;
    }
  assert(l <= r);
  const VarId center = (l + r) / 2;
  assert(center < pkgInfoVector.size());
  if (pkgInfoVector[center].pkgId == packageId)
    {
      fromPos = center;
      toPos = center;
      while(fromPos > 0 && pkgInfoVector[fromPos].pkgId == packageId)//VarId is unsigned, so all known overflow troubles is possible here, be careful!
	fromPos--;
      assert(fromPos < pkgInfoVector.size());
      if (pkgInfoVector[fromPos].pkgId != packageId)
	fromPos++;
      assert(pkgInfoVector[fromPos].pkgId == packageId);
      while(toPos < pkgInfoVector.size() && pkgInfoVector[toPos].pkgId == packageId)
	toPos++;
      assert(fromPos < toPos);
      return;
    }
  //We cannot find anything here;
  fromPos = 0;
  toPos = 0;
}

void PackageScopeContent::enhance(const PkgVector& pkgs, int flags)
{
  logMsg(LOG_DEBUG, "Trying to enhance package scope content with %zu new packages", pkgs.size());
  if (pkgs.empty())
    return;
  //First of all we should collect new package names;
  StringSet newNames;
  for(PkgVector::size_type i = 0;i < pkgs.size();i++)
    {
      const Pkg& pkg = pkgs[i];
      if (!checkName(pkg.name))
	newNames.insert(pkg.name);
      for(NamedPkgRelVector::size_type k = 0;k < pkg.requires.size();k++)
	if (!checkName(pkg.requires[k].pkgName))
	  newNames.insert(pkg.requires[k].pkgName);
      for(NamedPkgRelVector::size_type k = 0;k < pkg.provides.size();k++)
	if (!checkName(pkg.provides[k].pkgName))
	  newNames.insert(pkg.provides[k].pkgName);
      for(NamedPkgRelVector::size_type k = 0;k < pkg.obsoletes.size();k++)
	if (!checkName(pkg.obsoletes[k].pkgName))
	  newNames.insert(pkg.obsoletes[k].pkgName);
      for(NamedPkgRelVector::size_type k = 0;k < pkg.conflicts.size();k++)
	if (!checkName(pkg.conflicts[k].pkgName))
	  newNames.insert(pkg.conflicts[k].pkgName);
    } //for(pkgs);
  logMsg(LOG_DEBUG, "%zu new package names were found among the packages to enhance package scope content", newNames.size());
  //OK, we have new package names and can now add them and perform rearranging;
  for(StringSet::const_iterator it = newNames.begin();it != newNames.end();it++)
    names.push_back(*it);
  rearrangeNames();
  //What is the size of buffer we need for strings storing;
  size_t stringBufSize = 0;
  for(PkgVector::size_type i = 0;i < pkgs.size();i++)
    {
      const Pkg& pkg = pkgs[i];
      stringBufSize += pkg.version.length() + 1;
      stringBufSize += pkg.release.length() + 1;
      for(NamedPkgRelVector::size_type k = 0;k < pkg.requires.size();k++)
	stringBufSize += (pkg.requires[k].type == VerNone?0:pkg.requires[k].ver.size()) + 1;
      for(NamedPkgRelVector::size_type k = 0;k < pkg.provides.size();k++)
	stringBufSize += (pkg.provides[k].type == VerNone?0:pkg.provides[k].ver.size()) + 1;
      for(NamedPkgRelVector::size_type k = 0;k < pkg.conflicts.size();k++)
	stringBufSize += (pkg.conflicts[k].type == VerNone?0:pkg.conflicts[k].ver.size()) + 1;
      for(NamedPkgRelVector::size_type k = 0;k < pkg.obsoletes.size();k++)
	stringBufSize += (pkg.obsoletes[k].type == VerNone?0:pkg.obsoletes[k].ver.size()) + 1;
    }
  logMsg(LOG_DEBUG, "String buffer for new versions and releases must have length %zu bytes", stringBufSize);
  assert(stringBufSize > 0);
  std::auto_ptr<char> stringBuf(new char[stringBufSize]);
  size_t offset = 0;
  //We have done all strings preparing, adding new entries;
  for(PkgVector::size_type i = 0;i < pkgs.size();i++)
    {
      const Pkg& pkg = pkgs[i];
      PkgInfo info;
      assert(checkName(pkg.name));
      info.pkgId = strToPackageId(pkg.name);
      info.epoch = pkg.epoch;
      info.ver = placeStringInBuffer(stringBuf.get(), offset, pkg.version);
      info.release = placeStringInBuffer(stringBuf.get(), offset, pkg.release);
      info.buildTime = pkg.buildTime;
      info.flags = flags;
      addRelsForEnhancing(pkg.requires, info.requiresPos, info.requiresCount, stringBuf.get(), offset);
      addRelsForEnhancing(pkg.provides, info.providesPos, info.providesCount, stringBuf.get(), offset);
      addRelsForEnhancing(pkg.conflicts, info.conflictsPos, info.conflictsCount, stringBuf.get(), offset);
      addRelsForEnhancing(pkg.obsoletes, info.obsoletesPos, info.obsoletesCount, stringBuf.get(), offset);
      pkgInfoVector.push_back(info);
    }
  assert(offset == stringBufSize);
  addStringToAutoRelease(stringBuf.get());
  stringBuf.release();
  std::sort(pkgInfoVector.begin(), pkgInfoVector.end());
  logMsg(LOG_DEBUG, "Package scope content enhancing completed, now have %zu packages, %zu relations", pkgInfoVector.size(), relInfoVector.size());
}

void PackageScopeContent::addRelsForEnhancing(const NamedPkgRelVector& rels, size_t& pos, size_t& count, char* stringBuf, size_t& stringBufOffset)
{
  assert(stringBuf != NULL);
  if (rels.empty())
    {
      pos = 0;
      count = 0;
      return;
    }
  pos = relInfoVector.size();
  count = rels.size();
  for(NamedPkgRelVector::size_type i = 0;i < count;i++)
    {
      const NamedPkgRel& rel = rels[i];
      RelInfo info;
      assert(!rel.pkgName.empty());
      assert(checkName(rel.pkgName));
      info.pkgId = strToPackageId(rel.pkgName);
      if (rel.type != VerNone)
	{
	  info.type = rel.type;
	  info.ver = placeStringInBuffer(stringBuf, stringBufOffset, rel.ver);
	} else
	{
	  info.type = VerNone;
	  assert(rel.ver.empty());
	  placeStringInBuffer(stringBuf, stringBufOffset, "");
	}

      relInfoVector.push_back(info);
    }
}

bool PackageScopeContent::checkName(const std::string& name) const
{
  assert(!name.empty());
  if (names.empty())
    return 0;
  StringVector::size_type l = 0, r = names.size();
  while(l + 1 < r)
    {
      const StringVector::size_type c = (l + r) / 2;
      assert(c < names.size());
      if (names[c] == name)
	return 1;
      if (names[c] > name)
	r = c; else 
	l = c;
    }
  assert(l < names.size());
  return names[l] == name;
}

PackageId PackageScopeContent::strToPackageId(const std::string& name) const
{
  assert(!name.empty());
  assert(!names.empty());
  StringVector::size_type l = 0, r = names.size();
  while(l + 1 < r)
    {
      const StringVector::size_type c = (l + r) / 2;
      assert(c < names.size());
      if (names[c] == name)
	return c;
      if (names[c] > name)
	r = c; else 
	l = c;
    }
  assert(l < names.size());
  assert(names[l] == name);
  return l;
}

std::string PackageScopeContent::packageIdToStr(PackageId packageId) const
{
  assert(packageId < names.size());
  return names[packageId];
}

void PackageScopeContent::rearrangeNames()
{
  logMsg(LOG_DEBUG, "Performing names rearranging in package scope content, name count = %zu", names.size());
  if (names.size() < 2)
    return;
  StringVector newNames(names);
  std::sort(newNames.begin(), newNames.end());
  SizeVector newPlaces;
  newPlaces.resize(names.size());
  for(StringVector::size_type i = 0;i < names.size();i++)
    {
      StringVector::size_type res = 0;
      StringVector::size_type l = 0, r = newNames.size();
      while(1)
	{
	  assert(l <= r);
	  const StringVector::size_type c = (l + r) / 2;
	  assert(c < newNames.size());
	  if (newNames[c] == names [i])
	    {
	      res = c;
	      break;
	    }
	  if (newNames[c] > names[i])
	    r = c; else
	    l = c;
	} //while(1);
      newPlaces[i] = res;
    }
  for(PkgInfoVector::size_type i = 0;i < pkgInfoVector.size();i++)
    {
      assert(pkgInfoVector[i].pkgId < newPlaces.size());
      pkgInfoVector[i].pkgId = newPlaces[pkgInfoVector[i].pkgId];
    }
  for(RelInfoVector::size_type i = 0;i < relInfoVector.size();i++)
    {
      assert(relInfoVector[i].pkgId < newPlaces.size());
      relInfoVector[i].pkgId = newPlaces[relInfoVector[i].pkgId];
    }
  names = newNames;
  std::sort(pkgInfoVector.begin(), pkgInfoVector.end());
}

char* PackageScopeContent::placeStringInBuffer(char* buf, size_t& offset, const std::string& value)
{
  assert(buf != NULL);
  char* origPlace = &buf[offset];
  strcpy(&buf[offset], value.c_str());
  offset += value.length() + 1;
  return origPlace;
}

void PackageScopeContent::addStringToAutoRelease(char* str)
{
  assert(str != NULL);
  for(ConstCharStarVector::size_type i = 0;i < m_toAutoRelease.size();i++)
    if (m_toAutoRelease[i] == str)
      return;
  m_toAutoRelease.push_back(str);
}

void PackageScopeContent::releaseStrings()
{
  for(ConstCharStarVector::size_type i = 0;i < m_toAutoRelease.size();i++)
    delete[] m_toAutoRelease[i];
  m_toAutoRelease.clear();
}
