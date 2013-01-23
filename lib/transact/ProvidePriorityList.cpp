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
#include"ProvidePriorityList.h"

static void splitBySpaces(const std::string& str, StringList& items)
{
  items.clear();
  std::string s;
  for(std::string::size_type i = 0;i < str.length();i++)
    {
      if (BLANK_CHAR(str[i]))
	{
	  if (!s.empty())
	    items.push_back(s);
	  s.erase();
	  continue;
	}
      s += str[i];
    }
  if (!s.empty())
    items.push_back(s);
}

void ProvidePriorityList::load(const std::string& fileName)
{
  std::ifstream ifile(fileName.c_str());
  assert(ifile);//FIXME:exception;
  std::string line;
  while(1)
    {
      std::getline(ifile, line);
      if (!ifile)
	break;
      std::string::size_type k = 0;
      while(k < line.length() && BLANK_CHAR(line[k]))
	k++;
      if (k >= line.length())
	continue;
      if (line[k] == '#')
	continue;
      StringList items;
      splitBySpaces(line, items);
      if (items.size() < 2)
	continue;//FIXME:print a warning;
      StringList::const_iterator it = items.begin();
      assert(it != items.end());
      const std::string provideName = *it;
      it++;
      while(it != items.end())
	{
	  m_items.push_back(Item(provideName, *it));
	  it++;
	}
    }
}

void ProvidePriorityList::getPriority(const std::string& provideName, StringVector& pkgNamePriority) const
{
  pkgNamePriority.clear();
  for(ItemVector::size_type i = 0;i < m_items.size();i++)
    if (m_items[i].provideName == provideName)
      pkgNamePriority.push_back(m_items[i].packageName);
}
