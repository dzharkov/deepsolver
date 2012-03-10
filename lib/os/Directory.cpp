/*
   Copyright 2011-2012 ALT Linux
   Copyright 2011-2012 Dmitry V. Levin
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

bool Directory::isExist(const std::string& path)
{
  struct stat s;
  if (stat(path.c_str(), &s) == -1)
    return 0;
  return S_ISDIR(s.st_mode);
}

void Directory::ensureExists(const std::string& path)
{
  assert(!path.empty());
  std::string s;
  for(std::string::size_type i = 0;i < path.length();i++)
    {
      if (path[i] == '/')
	{
	  if (s.empty())
	    {
	      s = "/";
	      continue;
	    }
	  if (s[s.length() - 1] == '/')//No double slashes processing;
	    continue;
	  if (!isExist(s))
	    TRY_SYS_CALL(mkdir(s.c_str(), NEW_FILE_MODE) == 0, "mkdir(" + s + ")");
	  s += '/';
	  continue;
	}
      s += path[i];
    }
	  if (!isExist(s))
	    TRY_SYS_CALL(mkdir(s.c_str(), NEW_FILE_MODE) == 0, "mkdir(" + s + ")");
}

bool Directory::Iterator::moveNext()
{
  struct dirent* ent = readdir(m_dir);;
  if (ent)
    {
      m_currentName = ent->d_name;
      return 1;
    }
  closedir(m_dir);
  m_dir = NULL;
  return 0;
}

std::string Directory::Iterator::getName() const
{
  assert(m_dir);//m_currentName has a valid value;
  return m_currentName;
}

std::string Directory::Iterator::getFullPath() const
{
  assert(m_dir);//m_currentName has a valid value;
  return concatUnixPath(m_path, m_currentName);
}

std::auto_ptr<Directory::Iterator> Directory::enumerate(const std::string& path)
{
  DIR* dir = opendir(path.c_str());
  if (!dir)
    SYS_STOP("opendir(" + path + ")");
  return std::auto_ptr<Iterator>(new Iterator(path, dir));
}
