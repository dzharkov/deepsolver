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

#define NAME_STR "n="
#define EPOCH_STR "e="
#define VERSION_STR "v="
#define RELEASE_STR "r="
#define ARCH_STR "arch="
#define URL_STR "URL="
#define LICENSE_STR "lic="
#define PACKAGER_STR "pckgr="
#define SUMMARY_STr "summ="
#define DESCRIPTION_STR "descr="
#define SRCRPM_STR "src="
#define BUILDTIME_STR "btime="
#define PROVIDES_STR "p:"
#define REQUIRES_STR "r:"
#define CONFLICTS_STR "c:"
#define OBSOLETES_STR "o:"
#define CHANGELOG_STR "cl:"

std::string PkgSection::getPkgRelName(const std::string& line)
{
  //Name is stored at the beginning of line until first space without previously used backslash;
  std::string res;
  for(std::string::size_type i = 0;i < line.length();i++)
    {
      if (line[i] == '\\')
	{
	  i++;
	  if (i < line.length())
	    res += line[i]; else 
	    return res + "\\";
	  continue;
	}
      if (line[i] == ' ')
	return res;
      res += line[i];
    } //for();
  return res;
}

std::string PkgSection::saveNamedPkgRel(const NamedPkgRel& r)
{
  std::string name;
  for(std::string::size_type i = 0;i < r.pkgName.length();i++)
    {
      if (r.pkgName[i] == ' ' || r.pkgName[i] == '\\')
	name += "\\";
      name += r.pkgName[i];
    }
  std::ostringstream s;
  s << name;
  if (r.ver.empty())
    return s.str();
  const bool less = r.type & VerLess, equals = r.type & VerEquals, greater = r.type & VerGreater;
  assert(!less || !greater);
  std::string t;
  if (less)
    t += "<";
  if (greater)
    t += ">";
  if (equals)
    t += "=";
  s << " " << t << " " << r.ver;
  return s.str();
}

std::string PkgSection::saveFileName(const std::string& fileName)
{
  std::string s;
  for(std::string::size_type i = 0;i < fileName.length();i++)
    {
      if (fileName[i] == ' ' || fileName[i] == '\\')
	s += "\\";
      s += fileName[i];
    }
  return s;
}

std::string PkgSection::encodeMultiline(const std::string& s)
{
  std::string r;
  for(std::string::size_type i = 0;i < s.length();i++)
    {
      switch(s[i])
	{
	case '\\':
	  r += "\\\\";
	  break;
	case '\n':
	  r += "\\n";
	  break;
	case '\r':
	  continue;
	default:
	  r += s[i];
	}; //switch(s[i]);
    }
  return r;
}

std::string PkgSection::encodeChangeLogEntry(const ChangeLogEntry& entry)
{
  struct tm brTime;
  gmtime_r(&entry.time, &brTime);
  std::ostringstream s;
  s << (brTime.tm_year + 1900) << "-";
  if (brTime.tm_mon < 10)
    s << "0";
  s << (brTime.tm_mon + 1) << "-";
  if (brTime.tm_mday < 10)
    s << "0";
  s << brTime.tm_mday << std::endl;
  s << entry.name << std::endl;
  s << entry.text;
  return encodeMultiline(s.str());
}
