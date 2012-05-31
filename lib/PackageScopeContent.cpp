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
  if (provideMap.empty())
    return;
  ProvideMapItemVector::size_type l = 0, r = provideMap.size();
  while(l + 1 < r)
    {
      const ProvideMapItemVector::size_type center = (l + r) / 2;
      assert(center < provideMap.size());
      if (provideMap[center].provideId == provideId)
	{
	  ProvideMapItemVector::size_type fromPos, toPos;
	  fromPos = center;
	  toPos = center;
	  while(fromPos > 0 && provideMap[fromPos].provideId == provideId)//size_type is unsigned, so all known overflow troubles is possible here, be careful!
	    fromPos--;
	  assert(fromPos < provideMap.size());
	  if (provideMap[fromPos].provideId != provideId)
	    fromPos++;
	  assert(provideMap[fromPos].provideId == provideId);
	  while(toPos < provideMap.size() && provideMap[toPos].provideId == provideId)
	    toPos++;
	  assert(fromPos < toPos);
	  for(ProvideMapItemVector::size_type i = fromPos; i < toPos;i++)
	    providers.push_back(provideMap[i].pkgId);
	  return;
	}
      if (provideMap[center].provideId > provideId)
	r = center; else
	l = center;
    }
  assert(l <= r);
  const ProvideMapItemVector::size_type center = (l + r) / 2;
  assert(center < provideMap.size());
  if (provideMap[center].provideId == provideId)
    {
      ProvideMapItemVector::size_type fromPos, toPos;
      fromPos = center;
      toPos = center;
      while(fromPos > 0 && provideMap[fromPos].provideId == provideId)//size_type is unsigned, so all known overflow troubles is possible here, be careful!
	fromPos--;
      assert(fromPos < provideMap.size());
      if (provideMap[fromPos].provideId != provideId)
	fromPos++;
      assert(provideMap[fromPos].provideId == provideId);
      while(toPos < provideMap.size() && provideMap[toPos].provideId == provideId)
	toPos++;
      assert(fromPos < toPos);
      for(ProvideMapItemVector::size_type i = fromPos; i < toPos;i++)
	providers.push_back(provideMap[i].pkgId);
      return;
    }
  //We cannot find anything here;
  assert(providers.empty());
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

const PackageScopeContent::PkgInfoVector& PackageScopeContent::getPkgs() const
{
  return pkgInfoVector;
}

const PackageScopeContent::RelInfoVector& PackageScopeContent::getRels() const
{
  return relInfoVector;
}


void PackageScopeContent::rearrangeNames()
{
  //FIXME:Additional testing with small amount of data or without data at all;
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
  for(ProvideMapItemVector::size_type i = 0;i < provideMap.size();i++)
    {
      assert(provideMap[i].provideId < newPlaces.size());
      assert(provideMap[i].pkgId < newPlaces.size());
      provideMap[i].provideId = newPlaces[provideMap[i].provideId];
      provideMap[i].pkgId = newPlaces[provideMap[i].pkgId];
    }
  names = newNames;
  std::sort(pkgInfoVector.begin(), pkgInfoVector.end());
  std::sort(provideMap.begin(), provideMap.end());
}
