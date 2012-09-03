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
#include"Md5File.h"
#include"Md5.h"

#define IO_BUF_SIZE 1024

void Md5File::addItemFromFile(const std::string& fileName, const std::string& realPath)
{
  Md5 md5;
  md5.init();
  File f;
  f.openReadOnly(realPath);
  char buf[IO_BUF_SIZE];
  while(1)
    {
      const size_t count = f.read(buf, sizeof(buf));
      if (!count)
	break;
      md5.update(buf, count);
    }
  f.close();
  const std::string res = md5.commit();
  assert(res.length() == 32);
  Item item;
  item.checksum = res;
  item.fileName = fileName;
  items.push_back(item);
}

void Md5File::loadFromFile(const std::string& fileName)
{
  File f;
  f.openReadOnly(fileName);
  StringVector lines;
  f.readTextFile(lines);
  f.close();
  for(StringVector::size_type i = 0;i < lines.size();i++)
    {
      const std::string line = trim(lines[i]);
      if (line.empty())
	continue;
      if (line.length() < 35)
	throw Md5FileException(Md5FileErrorTooShortLine, fileName, i, line);
      Item item;
      for(std::string::size_type k = 0;k < 32;k++)
	item.checksum += line[k];
      for(std::string::size_type k = 34;k < line.length();k++)
	item.fileName = line[k];
      for(std::string::size_type k = 0;k < item.checksum.length();k++)
	{
	  if (item.checksum[k] >= 'A' && item.checksum[k] <= 'F')
	    item.checksum[k] = 'a' + (item.checksum[k] - 'A');
	  if ((item.checksum [k] >= 'a' && item.checksum[k] <= 'f') || (item.checksum[k] >= '0' && item.checksum [k] <= '9'))
	    continue;
	  throw Md5FileException(Md5FileErrorInvalidChecksumFormat, fileName, i, line);
	}
      items.push_back(item);
    }
}

void Md5File::saveToFile(const std::string& fileName) const
{
  std::string s;
  for(ItemVector::size_type i = 0;i < items.size();i++)
    s += (items[i].checksum + " *" + items[i].fileName + "\n");
  File f;
  f.open(fileName);
  f.write(s.c_str(), s.length());
  f.close();
}

bool Md5File::verifyItem(size_t itemIndex, const std::string& fileName) const
{
  assert(itemIndex < items.size());
  Md5 md5;
  md5.init();
  File f;
  f.openReadOnly(fileName);
  char buf[IO_BUF_SIZE];
  while(1)
    {
      const size_t count = f.read(buf, sizeof(buf));
      if (!count)
	break;
      md5.update(buf, count);
    }
  f.close();
  return items[itemIndex].checksum == md5.commit();
}
