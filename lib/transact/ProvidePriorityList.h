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

#ifndef DEEPSOLVER_PROVIDE_PRIORITY_LIST_H
#define DEEPSOLVER_PROVIDE_PRIORITY_LIST_H

class ProvidePriorityList
{
public:
  ProvidePriorityList() {}
  ~ProvidePriorityList() {}

public:
  void load(const std::string& fileName);

  //The first item in result is the item with the highest priority;
  void getPriority(const std::string& provideName, StringVector& pkgNamePriority) const;

private:
  struct Item
  {
    Item(const std::string& provide, const std::string& pkg)
      : provideName(provide),
	packageName(pkg) {}

    std::string provideName;
    std::string packageName;
  }; //struct Item;

  typedef std::vector<Item> ItemVector;
  typedef std::list<Item> ItemList;

private:
  ItemVector m_items;
}; //class ProvidePriorityList;

#endif //DEEPSOLVER_PROVIDE_PRIORITY_LIST_H;
