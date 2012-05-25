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

void PackageScopeContent::locateRange(const PackageScopeContent::PkgInfoVector& pkgs, PackageId packageId, VarId& fromPos, VarId& toPos )
{
  if (pkgs.empty())
    {
      fromPos = 0;
      toPos = 0;
      return;
    }
  VarId l = 0, r = pkgs.size();
  while(l + 1 < r)
    {
      const VarId center = (l + r) / 2;
      assert(center < pkgs.size());
      if (pkgs[center].pkgId == packageId)
	{
	  fromPos = center;
	  toPos = center;
	  while(fromPos > 0 && pkgs[fromPos].pkgId == packageId)//VarId is unsigned, so all known overflow troubles is possible here, be careful!
	    fromPos--;
	  assert(fromPos < pkgs.size());
	  if (pkgs[fromPos].pkgId != packageId)
	    fromPos++;
	  assert(pkgs[fromPos].pkgId == packageId);
	  while(toPos < pkgs.size() && pkgs[toPos].pkgId == packageId)
	    toPos++;
	  assert(fromPos < toPos);
	  return;
	}
      if (pkgs[center].pkgId > packageId)
	r = center; else
	l = center;
    }
  assert(l <= r);
  const VarId center = (l + r) / 2;
  assert(center < pkgs.size());
  if (pkgs[center].pkgId == packageId)
    {
      fromPos = center;
      toPos = center;
      while(fromPos > 0 && pkgs[fromPos].pkgId == packageId)//VarId is unsigned, so all known overflow troubles is possible here, be careful!
	fromPos--;
      assert(fromPos < pkgs.size());
      if (pkgs[fromPos].pkgId != packageId)
	fromPos++;
      assert(pkgs[fromPos].pkgId == packageId);
      while(toPos < pkgs.size() && pkgs[toPos].pkgId == packageId)
	toPos++;
      assert(fromPos < toPos);
      return;
    }
  //We cannot find anything here;
  fromPos = 0;
  toPos = 0;
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
  if (m_names.empty())
    return 0;
  StringVector::size_type l = 0, r = m_names.size();
  while(l + 1 < r)
    {
      const StringVector::size_type c = (l + r) / 2;
      assert(c < m_names.size());
      if (m_names[c] == name)
	return 1;
      if (m_names[c] > name)
	r = c; else 
	l = c;
    }
  assert(l < m_names.size());
  return m_names[l] == name;
}

PackageId PackageScopeContent::strToPackageId(const std::string& name) const
{
  assert(!name.empty());
  assert(!m_names.empty());
  StringVector::size_type l = 0, r = m_names.size();
  while(l + 1 < r)
    {
      const StringVector::size_type c = (l + r) / 2;
      assert(c < m_names.size());
      if (m_names[c] == name)
	return c;
      if (m_names[c] > name)
	r = c; else 
	l = c;
    }
  assert(l < m_names.size());
  assert(m_names[l] == name);
  return l;
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
