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
#include"TextFiles.h"

struct MapItem
{
  MapItem() {}
  MapItem(const std::string& pkgName, const std::string& provideName)
    : pkg(pkgName), provide(provideName) {}

  bool operator ==(const MapItem& item) const
  {
    return pkg == item.pkg && provide == item.provide;
  }

  bool operator !=(const MapItem& item) const
  {
    return pkg != item.pkg || provide != item.provide;
  }

  std::string pkg, provide;
}; //struct MapItem;

typedef std::list<MapItem> MapItemList;

MapItemList list1;//By rpms.data;
MapItemList list2;//By provides.data;

std::auto_ptr<AbstractTextFileReader> openIndexFile(const std::string& fileName)
{
  if (checkExtension(fileName, ".gz"))
    return createTextFileReader(TextFileGZip, fileName);
  return createTextFileReader(TextFileStd, fileName);
}

//Package is string beginning until first space without previously used backslash;
std::string extractPackageName(const std::string& line)
{
  std::string s;
  for(std::string::size_type i = 0;i < line.length();i++)
    {
      if (line[i] == '\\')
	{
	  if (i + 1 < line.length())
	      s += line[++i]; else
	    s += "\\";
	  continue;
	}
      if (line[i] == ' ')
	return s;
      s += line[i];
    }
  return s;
}

void processPkgFile(const std::string& fileName)
{
  std::cout << "Reading " << fileName << "..." << std::endl;
  std::auto_ptr<AbstractTextFileReader> reader = openIndexFile(fileName);
  std::string line, name;
  while(reader->readLine(line))
    {
      std::string tail;
      if (stringBegins(line, "n=", tail))
	{
	  name = tail;
	  continue;
	}
      if (stringBegins(line, "p:", tail))
	{
	  std::string pkgName = extractPackageName(tail);
	  assert(!pkgName.empty());
	  list1.push_back(MapItem(name, pkgName));
	}
    }
}

void processProvidesFile(const std::string& fileName)
{
  std::cout << "Reading " << fileName << "..." << std::endl;
  std::auto_ptr<AbstractTextFileReader> reader = openIndexFile(fileName);
  std::string line, provideName;
  bool wasEmpty = 1;
  while(reader->readLine(line))
    {
      if (line.empty())
	{
	  wasEmpty = 1;
	  continue;
	}
      if (wasEmpty && line.length() > 2 && line[0] == '[' && line[line.length() - 1] == ']')
	{
	  provideName.resize(line.length() - 2);
	  for(std::string::size_type i = 1;i < line.length() - 1;i++)
	    provideName[i - 1] = line[i];
	  wasEmpty = 0;
	  continue;
	}
      assert(!provideName.empty());
      list2.push_back(MapItem(line, provideName));
      wasEmpty = 0;
    }
}

int main(int argc, char* argv[])
{
  assert(argc == 3);
  processPkgFile(argv[1]);
  processProvidesFile(argv[2]);
  std::cout << "First list size: " << list1.size() << std::endl;
  std::cout << "Second list size: " << list2.size() << std::endl;
  assert(list1.size() == list2.size());
  for(MapItemList::const_iterator it1 = list1.begin();it1 != list1.end();it1++)
    {
      MapItemList::const_iterator it2 = list2.begin();
      while (it2 != list2.end() && *it1 != *it2)
	it2++;
      assert(it2 != list2.end());
    }
  for(MapItemList::const_iterator it2 = list2.begin();it2 != list2.end();it2++)
    {
      MapItemList::const_iterator it1 = list1.begin();
      while (it1 != list1.end() && *it2 != *it1)
	it1++;
      assert(it1 != list1.end());
    }
  return 0;
}
