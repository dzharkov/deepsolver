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

#ifndef DEEPSOLVER_PACKAGE_NAME_SORTING_H
#define DEEPSOLVER_PACKAGE_NAME_SORTING_H

#include"AbstractPackageBackEnd.h"

struct PackageNameSortItem
{
  PackageNameSortItem(const AbstractPackageBackEnd* b,
		   VarId v,
		   const std::string& n)
    : backEnd(b),
      varId(v),
      name(n) 
  {
    assert(backEnd != NULL);
  }

  bool operator <(const PackageNameSortItem& item) const
  {
    return backEnd->versionCompare(name, item.name) < 0;
  }

  bool operator >(const PackageNameSortItem& item) const
  {
    return backEnd->versionCompare(name, item.name) > 0;
  }

  const AbstractPackageBackEnd* backEnd;
  VarId varId;
  std::string name;
}; //struct PrioritySortItem;

typedef std::vector<PackageNameSortItem> PackageNameSortItemVector;
typedef std::list<PackageNameSortItem> PackageNameSortItemList;

#endif //DEEPSOLVER_PACKAGE_NAME_SORTING_H;
