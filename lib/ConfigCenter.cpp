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

#include"ConfigCenter.h"

static std::string collectFiles(const std::string& dirName)
{
  StringVector items;
  DIR* dir = opendir(dirName.c_str());
  if (dir == NULL)
    return std::string();
  struct dirent* ent;
  while ((ent = readdir(dir)) != NULL)
    {
      std::string name(ent->d_name);
      if (name == "." || name == "..")
	continue;
      struct stat s;
      std::string nameToCheck;
      if (dirName.empty() || dirName[dirName.length() - 1] == '/')//FIXME:
	nameToCheck = dirName + name; else
	nameToCheck = dirName + "/" + name;
      if (stat(nameToCheck.c_str(), &s) != 0)
	continue;
      if (!S_ISREG(s.st_mode))//FIXME:
	continue;
      items.push_back(name);
    }
  closedir(dir);
  std::sort(items.begin(), items.end());
  std::string res;
  for(StringVector::size_type i = 0;i < items.size();i++)
    {
      std::string nameToRead;
      if (dirName.empty() || dirName[dirName.length() - 1] == '/')
	nameToRead = dirName + items[i]; else
	nameToRead = dirName + "/" + items[i];//FIXME:
      char c;
      std::string s;
      std::ifstream f(nameToRead.c_str());
      if (!f)
	continue;
      while(f.get(c))
	s += c;
      s += '\n';
      res += s;
    } //for files;
  return res;
}

  void onConfigFileValue(const StringVector& path, 
			 const std::string& sectArg,
			 const std::string& value,
			 bool adding)
  {
    assert(!path.empty());
    std::cout << path[0];
    if (!sectArg.empty())
      std::cout << " \"" << sectArg << "\"";
    for(size_t i = 1;i < path.size();i++)
      std::cout << "." << path[i];
    if (adding)
      std::cout << " += "; else 
      std::cout << " = ";
    std::cout << value << std::endl;
  }

