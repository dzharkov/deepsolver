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
#include"PkgSection.h"

#define NAME_STR "n="
#define EPOCH_STR "e="
#define VERSION_STR "v="
#define RELEASE_STR "r="
#define ARCH_STR "arch="
#define URL_STR "URL="
#define LICENSE_STR "lic="
#define PACKAGER_STR "pckgr="
#define GROUP_STR "g="
#define SUMMARY_STr "summ="
#define DESCRIPTION_STR "descr="
#define SRCRPM_STR "src="
#define BUILDTIME_STR "btime="
#define PROVIDES_STR "p:"
#define REQUIRES_STR "r:"
#define CONFLICTS_STR "c:"
#define OBSOLETES_STR "o:"
#define CHANGELOG_STR "cl:"

static std::string encodeMultiline(const std::string& s);
static std::string encodeChangeLogEntry(const ChangeLogEntry& entry);
static std::string saveNamedPkgRel(const NamedPkgRel& r);
static std::string saveFileName(const std::string& fileName);
static bool fileFromDirs(const std::string& fileName, const StringVector& dirs);
static std::string extractPkgRelName(const std::string& line);

static bool translateRelType(const std::string& str, NamedPkgRel& rel);
static bool parsePkgRel(const std::string& str, NamedPkgRel& rel);

std::string PkgSection::saveBaseInfo(const PkgFile& pkgFile, const StringVector& filterProvidesByDirs)
{
  std::ostringstream ss;
  ss << "[" << File::baseName(pkgFile.fileName) << "]" << std::endl;
  ss << NAME_STR << pkgFile.name << std::endl;
  ss << EPOCH_STR << pkgFile.epoch << std::endl;
  ss << VERSION_STR << pkgFile.version << std::endl;
  ss << RELEASE_STR << pkgFile.release << std::endl;
  ss << ARCH_STR << pkgFile.arch << std::endl;
  ss << BUILDTIME_STR  << pkgFile.buildTime << std::endl;
  for(NamedPkgRelVector::const_iterator it = pkgFile.provides.begin();it != pkgFile.provides.end();it++)
    {
      /*
       * The following operation must be done in both cases: in filtering by
       * references mode and without filtering at all. If there is no any filtering we just
       * saving all provides, if filtering is enabled we will proceed real
       * filtering on additional phase.
       */
      ss << PROVIDES_STR << saveNamedPkgRel(*it) << std::endl;
    }
  for(StringVector::size_type i = 0;i < pkgFile.fileList.size();i++)
    if (filterProvidesByDirs.empty() || fileFromDirs(pkgFile.fileList[i], filterProvidesByDirs))
      ss << PROVIDES_STR << saveFileName(pkgFile.fileList[i]) << std::endl;
  for(NamedPkgRelVector::const_iterator it = pkgFile.requires.begin();it != pkgFile.requires.end();it++)
      //FIXME:      if (m_requireFilter.excludeRequire(it->pkgName))
    ss << REQUIRES_STR << saveNamedPkgRel(*it) << std::endl;
  for(NamedPkgRelVector::const_iterator it = pkgFile.conflicts.begin();it != pkgFile.conflicts.end();it++)
    ss << CONFLICTS_STR << saveNamedPkgRel(*it) << std::endl;
  for(NamedPkgRelVector::const_iterator it = pkgFile.obsoletes.begin();it != pkgFile.obsoletes.end();it++)
    ss << OBSOLETES_STR << saveNamedPkgRel(*it) << std::endl;
  ss << std::endl;
  return ss.str();
}

std::string PkgSection::saveDescr(const PkgFile& pkgFile, bool saveChangeLog)
{
  std::ostringstream ss;
  ss << "[" << File::baseName(pkgFile.fileName) << "]" << std::endl;
  ss << URL_STR << pkgFile.url << std::endl;
  ss << LICENSE_STR << pkgFile.license << std::endl;
  ss << PACKAGER_STR << pkgFile.packager << std::endl;
  ss << GROUP_STR << pkgFile.group << std::endl;
  ss << SUMMARY_STr << pkgFile.summary << std::endl;
  ss << DESCRIPTION_STR << encodeMultiline(pkgFile.description) << std::endl;
  ss << SRCRPM_STR << pkgFile.srcRpm << std::endl;
  if (saveChangeLog)
    {
      for(ChangeLog::size_type i = 0;i < pkgFile.changeLog.size();i++)
	ss << CHANGELOG_STR << encodeChangeLogEntry(pkgFile.changeLog[i]) << std::endl;
    }
  ss << std::endl;
  return ss.str();
}

std::string PkgSection::saveFileList(const PkgFile& pkgFile)
{
  std::ostringstream ss;
  ss << "[" << pkgFile.fileName << "]" << std::endl;
  for(StringVector::size_type i = 0;i < pkgFile.fileList.size();i++)
    ss << pkgFile.fileList[i] << std::endl;
  ss << std::endl;
  return ss.str();
}

bool PkgSection::isProvidesLine(const std::string& line, std::string& pkgName)
{
  std::string tail;
  if (!stringBegins(line, PROVIDES_STR, tail))
    return 0;
  pkgName = extractPkgRelName(tail);
  return 1;
}

std::string PkgSection::getPkgFileName(const std::string& section)
{
  std::string::size_type i = 0;
  while (i < section.length() && section[i] != '[')
    i++;
  if (i >= section.length())
    return "";
  std::string fileName;
  i++;
  while(i < section.length() && section[i] != ']')
    fileName += section[i++];
  return fileName;
}

void PkgSection::extractProvidesReferences(const std::string& section, StringSet& refs)
{
  std::string line;
  for(std::string::size_type i = 0;i < section.length();i++)
    {
      if (section[i] == '\r')
	continue;
      if (section[i] != '\n')
	{
	  line += section[i];
	  continue;
	}
      std::string tail;
      if (stringBegins(line, REQUIRES_STR, tail))
	refs.insert(extractPkgRelName(tail));
      if (stringBegins(line, CONFLICTS_STR, tail))
	refs.insert(extractPkgRelName(tail));
      line.erase();
    }
  if (line.empty())
    return;
  std::string tail;
  if (stringBegins(line, REQUIRES_STR, tail))
    refs.insert(extractPkgRelName(tail));
  if (stringBegins(line, CONFLICTS_STR, tail))
    refs.insert(extractPkgRelName(tail));
}

bool PkgSection::parsePkgFileSection(const StringVector& sect, PkgFile& pkgFile, size_t& invalidLineNum, std::string& invalidLineValue)
{
  assert(!sect.empty());
  pkgFile.fileName = sect[0];
  if (pkgFile.fileName.length() <= 2)//FIXME:must be exception;
    {
      invalidLineNum = 0;
      invalidLineValue = sect[0];
      return 0;
    }
  for(std::string::size_type k = 1;k < pkgFile.fileName.length();k++)
    pkgFile.fileName[k - 1] = pkgFile.fileName[k];
  pkgFile.fileName.resize(pkgFile.fileName.size() - 2);
  for(StringVector::size_type i = 0;i < sect.size();i++)
    {
      const std::string& line = sect[i];
      if(line.empty())
	{
	  invalidLineNum = i;
	  invalidLineValue.erase();
	  return 0;
	}
      std::string tail;
      if (stringBegins(line, NAME_STR, tail))
	{
	pkgFile.name = tail;
	continue;
	}
      if (stringBegins(line, EPOCH_STR, tail))
	{
	  std::istringstream ss(tail);
	  if (!(ss >> pkgFile.epoch))
	    {
	      invalidLineNum = i;
	      invalidLineValue = line;
	      return 0;
	    }
	  continue;
	}
      if (stringBegins(line, VERSION_STR, tail))
	{
	  pkgFile.version = tail;
	  continue;
	}
      if (stringBegins(line, RELEASE_STR, tail))
	{
	  pkgFile.release = tail;
	  continue;
	}
      if (stringBegins(line, BUILDTIME_STR, tail))
	{
	  std::istringstream ss(tail);
	  if (!(ss >> pkgFile.buildTime))
	    {
	      invalidLineNum = i;
	      invalidLineValue = line;
	      return 0;
	    }
	  continue;
	}
      if (stringBegins(line, REQUIRES_STR, tail))
	{
	  NamedPkgRel r;
	  if (!parsePkgRel(tail, r))
	    {
	      invalidLineNum = i;
	      invalidLineValue = line;
	      return 0;
	    }
	  pkgFile.requires.push_back(r);
	  continue;
	}
      if (stringBegins(line, CONFLICTS_STR, tail))
	{
	  NamedPkgRel r;
	  if (!parsePkgRel(tail, r))
	    {
	      invalidLineNum = i;
	      invalidLineValue = line;
	      return 0;
	    }
	  pkgFile.conflicts.push_back(r);
	  continue;
	}
      if (stringBegins(line, PROVIDES_STR, tail))
	{
	  NamedPkgRel r;
	  if (!parsePkgRel(tail, r))
	    {
	      invalidLineNum = i;
	      invalidLineValue = line;
	      return 0;
	    }
	  pkgFile.provides.push_back(r);
	  continue;
	}
      if (stringBegins(line, OBSOLETES_STR, tail))
	{
	  NamedPkgRel r;
	  if (!parsePkgRel(tail, r))
	    {
	      invalidLineNum = i;
	      invalidLineValue = line;
	      return 0;
	    }
	  pkgFile.obsoletes.push_back(r);
	  continue;
	}
    }
  return 1;
}

std::string encodeMultiline(const std::string& s)
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

std::string encodeChangeLogEntry(const ChangeLogEntry& entry)
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

std::string saveNamedPkgRel(const NamedPkgRel& r)
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

std::string saveFileName(const std::string& fileName)
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

bool fileFromDirs(const std::string& fileName, const StringVector& dirs)
{
  std::string tail;
  for(StringVector::const_iterator it = dirs.begin();it != dirs.end();it++)
    if (stringBegins(fileName, *it, tail))
      return 1;
  return 0;
}

std::string extractPkgRelName(const std::string& line)
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
	    {
	      logMsg(LOG_WARNING, "Found abnormal line in text format binary: \'%s\'", line.c_str());
	      return res + "\\";
	    }
	  continue;
	}
      if (line[i] == ' ')
	return res;
      res += line[i];
    } //for();
  return res;
}


bool translateRelType(const std::string& str, NamedPkgRel& rel)
{
  if(str != "<" &&
     str != ">" &&
     str != "=" &&
     str != "<=" &&
     str != ">=")
    return 0;
  rel.type = 0;
  if (str == "<" || str == "<=")
    rel.type |= VerLess;
  if (str == "<=" ||
      str == "=" ||
      str == ">=")
    rel.type |= VerEquals;
  if (str == ">" || str == ">=")
    rel.type |= VerGreater;
  return 0;
}

bool parsePkgRel(const std::string& str, NamedPkgRel& rel)
{
  rel = NamedPkgRel();
  std::string::size_type i = 0;
  //Extracting package name;
  while(i < str.length() && str[i] != ' ')
    {
      if (str[i] == '\\')
	{
	  if (i + 1 >= str.length())
	    {
	      rel.pkgName += "\\";
	      return 1;
	    }
	  assert(i + 1 < str.length());
	  rel.pkgName += str[i + 1];
	  i += 2;
	  continue;
	} //backslash;
      rel.pkgName += str[i++];
    }
  if (i >= str.length())
    return 1;
  assert(str[i] == ' ');
  i++;
  //Here must be <, =, > or any their combination;
  if (i + 1 >= str.length())
    return 0;
  std::string r;
  r += str[i];
  if (str[i + 1] != ' ')
    {
      r += str[i + 1];
      i++;
    }
  i++;
  if (!translateRelType(r, rel))
    return 0;
  if (i >= str.length() || str[i] != ' ');//FIXME:must be an exception
  return 0;
  i++;
  //Here we expect package version;
  rel.ver.erase();
  while(i < str.length())
    rel.ver += str[i++];
}

