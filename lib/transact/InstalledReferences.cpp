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
#include"InstalledReferences.h"

void InstalledReferences::searchReferencesTo(PackageId refTo, VarIdVector& res)
{
  assert(refTo != BAD_PACKAGE_ID);
  res.clear();
  if (m_references.empty())
    return;
  if (m_references.size() == 1)
    {
      if (m_references[0].refTo == refTo)
	res.push_back(m_references[0].refFrom);
      return;
    }
  ReferenceVector::size_type l = 0, r = m_references.size();
  while(l + 1 < r)
    {
      const ReferenceVector::size_type center = (l + r) / 2;
      assert(center < m_references.size());
      if (m_references[center].refTo == refTo)
	{
	  ReferenceVector::size_type fromPos, toPos;
	  fromPos = center;
	  toPos = center;
	  while(fromPos > 0 && m_references[fromPos].refTo == refTo)//size_type is unsigned, so all known overflow troubles is possible here, be careful!
	    fromPos--;
	  assert(fromPos < m_references.size());
	  if (m_references[fromPos].refTo != refTo)
	    fromPos++;
	  assert(m_references[fromPos].refTo == refTo);
	  while(toPos < m_references.size() && m_references[toPos].refTo == refTo)
	    toPos++;
	  assert(fromPos < toPos);
	  for(ReferenceVector::size_type i = fromPos; i < toPos;i++)
	    res.push_back(m_references[i].refFrom);
	  return;
	}
      if (m_references[center].refTo > refTo)
	r = center; else
	l = center;
    }
  assert(l <= r);
  const ReferenceVector::size_type center = (l + r) / 2;
  assert(center < m_references.size());
  if (m_references[center].refTo == refTo)
    {
      ReferenceVector::size_type fromPos, toPos;
      fromPos = center;
      toPos = center;
      while(fromPos > 0 && m_references[fromPos].refTo == refTo)//size_type is unsigned, so all known overflow troubles is possible here, be careful!
	fromPos--;
      assert(fromPos < m_references.size());
      if (m_references[fromPos].refTo != refTo)
	fromPos++;
      assert(m_references[fromPos].refTo == refTo);
      while(toPos < m_references.size() && m_references[toPos].refTo == refTo)
	toPos++;
      assert(fromPos < toPos);
      for(ReferenceVector::size_type i = fromPos; i < toPos;i++)
	res.push_back(m_references[i].refFrom);
      return;
    }
  //We cannot find anything here;
  assert(res.empty());
}

void InstalledReferences::add(PackageId refTo, VarId refFrom)
{
  assert(refTo != BAD_VAR_ID && refFrom != BAD_VAR_ID);
  m_references.push_back(Reference(refTo, refFrom));
}

void InstalledReferences::commit()
{
  std::sort(m_references.begin(), m_references.end());
}
