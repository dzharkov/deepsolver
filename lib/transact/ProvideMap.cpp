/*
   Copyright 2011-2013 ALT Linux
   Copyright 2011-2013 Michael Pozhidaev

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
#include"ProvideMap.h"

void ProvideMap::searchProviders(PackageId provideId, VarIdVector& res) const
{
  assert(provideId != BAD_PACKAGE_ID);
  res.clear();
  if (m_items.empty())
    return;
  if (m_items.size() == 1)
    {
      if (m_items[0].provideId == provideId)
	res.push_back(m_items[0].providedBy);
      return;
    }
  ItemVector::size_type l = 0, r = m_items.size();
  while(l + 1 < r)
    {
      const ItemVector::size_type center = (l + r) / 2;
      assert(center < m_items.size());
      if (m_items[center].provideId == provideId)
	{
	  ItemVector::size_type fromPos, toPos;
	  fromPos = center;
	  toPos = center;
	  while(fromPos > 0 && m_items[fromPos].provideId == provideId)//size_type is unsigned, so all known overflow troubles is possible here, be careful!
	    fromPos--;
	  assert(fromPos < m_items.size());
	  if (m_items[fromPos].provideId != provideId)
	    fromPos++;
	  assert(m_items[fromPos].provideId == provideId);
	  while(toPos < m_items.size() && m_items[toPos].provideId == provideId)
	    toPos++;
	  assert(fromPos < toPos);
	  for(ItemVector::size_type i = fromPos; i < toPos;i++)
	    res.push_back(m_items[i].providedBy);
	  return;
	}
      if (m_items[center].provideId > provideId)
	r = center; else
	l = center;
    }
  assert(l <= r);
  const ItemVector::size_type center = (l + r) / 2;
  assert(center < m_items.size());
  if (m_items[center].provideId == provideId)
    {
      ItemVector::size_type fromPos, toPos;
      fromPos = center;
      toPos = center;
      while(fromPos > 0 && m_items[fromPos].provideId == provideId)//size_type is unsigned, so all known overflow troubles is possible here, be careful!
	fromPos--;
      assert(fromPos < m_items.size());
      if (m_items[fromPos].provideId != provideId)
	fromPos++;
      assert(m_items[fromPos].provideId == provideId);
      while(toPos < m_items.size() && m_items[toPos].provideId == provideId)
	toPos++;
      assert(fromPos < toPos);
      for(ItemVector::size_type i = fromPos; i < toPos;i++)
	res.push_back(m_items[i].providedBy);
      return;
    }
  //We cannot find anything here;
  assert(res.empty());
}

void ProvideMap::fillWith(const PackageScopeContent& content)
{
  const clock_t startClock = clock();
  const PackageScopeContent::PkgInfoVector& pkgs = content.pkgInfoVector;
  const PackageScopeContent::RelInfoVector& rels = content.relInfoVector;
  for(size_t i = 0;i < pkgs.size();i++)
    {
      const size_t pos = pkgs[i].providesPos, count = pkgs[i].providesCount;
      for(size_t k = 0;k < count;k++)
	m_items.push_back(Item(rels[pos + k].pkgId, i));
    }
  std::sort(m_items.begin(), m_items.end());
  const double duration = (double)(clock() - startClock) / CLOCKS_PER_SEC;
  logMsg(LOG_DEBUG, "Provide map was filled with %zu entries in %f sec", m_items.size(), duration);
}

void ProvideMap::add(PackageId provideId, VarId providedBy)
{
  assert(provideId != BAD_PACKAGE_ID && providedBy != BAD_VAR_ID);
  m_items.push_back(Item(provideId, providedBy));
}

void ProvideMap::commit()
{
  std::sort(m_items.begin(), m_items.end());
}
