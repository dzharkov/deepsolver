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

#ifndef DEEPSOLVER_PROVIDE_MAP_H
#define DEEPSOLVER_PROVIDE_MAP_H

#include"PackageScopeContent.h"

class ProvideMap
{
public:
  ProvideMap() {}
  virtual ~ProvideMap() {}

public:
  void fillWith(const PackageScopeContent& content);
  void add(PackageId provideId, VarId providedBy);
  void commit();
  void searchProviders(PackageId provideId, VarIdVector& res) const;

private:
  struct Item
  {
    Item()
      : provideId(BAD_PACKAGE_ID), providedBy(BAD_VAR_ID) {}

    Item(PackageId pkgId, VarId prBy)
      : provideId(pkgId), providedBy(prBy) {}

    bool operator <(const Item& r) const
    {
      return provideId < r.provideId;
    }

    bool operator >(const Item& r) const
    {
      return provideId > r.provideId;
    }

    PackageId provideId;
    VarId providedBy;
  }; //struct Item;

  typedef std::vector<Item> ItemVector;

private:
  ItemVector m_items;
}; //class Item;

#endif //DEEPSOLVER_PROVIDE_MAP_H;
