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

bool Directory::ensureExistsAndEmpty(const std::string& name, bool needEraseContent)
{
  assert(!name.empty());
  ensureExists(name);
  if (!needEraseContent)
    {
      std::auto_ptr<Iterator> it = enumerate(name);
      return !it->moveNext();
    }
  eraseContent(name);
  return 1;
}

void Directory::eraseContent(const std::string& name)
{
  std::auto_ptr<Iterator> it = enumerate(name);
  while(it->moveNext())
    {
      if (it->getName() == "." || it->getName() == "..")
	continue;
      const std::string& path = it->getFullPath();
      if (File::isDir(path))
	{
	  eraseContent(path);
	  remove(path);
	  continue;
	}
      File::unlink(path);
    }
}

void Directory::remove(const std::string& name)
{
  assert(!name.empty());
  TRY_SYS_CALL(rmdir(name.c_str()) == 0, "rmdir(" + name + ")");
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
  return Directory::mixNameComponents(m_path, m_currentName);//FIXME:
}

std::auto_ptr<Directory::Iterator> Directory::enumerate(const std::string& path)
{
  DIR* dir = opendir(path.c_str());
  if (!dir)
    SYS_STOP("opendir(" + path + ")");
  return std::auto_ptr<Iterator>(new Iterator(path, dir));
}

std::string Directory::mixNameComponents(const std::string& part1, const std::string& part2)
{
  if (part1.empty() && part2.empty())
    return "";
  if (part1.empty())
    return part2;
  if (part2.empty())
    return part1;
  const std::string::value_type lastChar1 = part1[part1.length() - 1], firstChar2 = part2[0];
  if (lastChar1 != '/' && firstChar2 != '/')
    return part1 + "/" + part2;
  if ((lastChar1 == '/' && firstChar2 != '/') || (lastChar1 != '/' && firstChar2 == '/'))
    return part1 + part2;
  assert(lastChar1 == '/' && firstChar2 == '/');
  std::string res = part1;
  res.resize(res.length() - 1);
  return res + part2;
}
