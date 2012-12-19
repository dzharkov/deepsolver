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

#ifndef DEEPSOLVER_MD5FILE_H
#define DEEPSOLVER_MD5FILE_H

class Md5File
{
public:
class Item
{
public:
  std::string checksum;
  std::string fileName;
}; //class Item;

public:
  typedef std::vector<Item> ItemVector;
  typedef std::list<Item> ItemList;

public:
  /**\brief The default constructor*/
  Md5File() {}

  /**\brief The destructor*/
  virtual ~Md5File() {}

public:
  void addItemFromFile(const std::string& fileName, const std::string& realPath);
  void loadFromString(const std::string& str, const std::string& fileName);
  void loadFromFile(const std::string& fileName);
  void saveToFile(const std::string& fileName) const;
  bool verifyItem(size_t itemIndex, const std::string& fileName) const;
  bool verifyItemByString(size_t itemIndex, const std::string& str) const
  void removeItem(const std::string& fileName);

public:
  ItemVector items;
}; //class Md5File;

#endif //DEEPSOLVER_MD5FILE_H;
