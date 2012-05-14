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

/*
 * This file contains first preliminary implementation of reading class
 * for repository index data in text format. Due to reasons of debugging
 * simplicity all errors caused by invalid input data are handled as
 * asserts but in future all of them must be exceptions. Most of asserts
 * to be replaced by exceptions are marked by the corresponding "FIXME"
 * comments.
 */

#include"deepsolver.h"
#include"RepoIndexTextFormatReader.h"
#include"IndexCore.h"

static   void translateRelType(const std::string& str, NamedPkgRel& rel)
{
  assert(str == "<" || str == ">" || str == "=" || str == "<=" || str == ">=");
  rel.type = 0;
  if (str == "<" || str == "<=")
    rel.type |= VerLess;
  if (str == "<=" ||
      str == "=" ||
      str == ">=")
    rel.type |= VerEquals;
  if (str == ">" || str == ">=")
    rel.type |= VerGreater;
}

static void parsePkgRel(const std::string& str, NamedPkgRel& rel)
{
  std::string::size_type i = 0;
  //Extracting package name;
  while(i < str.length() && str[i] != ' ')
    {
      if (str[i] == '\\')
	{
	  if (i + 1 >= str.length())
	    {
	      rel.pkgName += "\\";
	      return;
	    }
	  assert(i + 1 < str.length());
	  rel.pkgName += str[i + 1];
	  i += 2;
	  continue;
	} //backslash;
      rel.pkgName += str[i++];
    }
  if (i >= str.length())
    return;
  i++;
  //Here must be <, =, > or any their combination;
  assert(i + 1 < str.length());//FIXME:it must be an exception;
  std::string r;
  r += str[i];
  if (str[i + 1] != ' ')
    {
      r += str[i + 1];
      i++;
    }
  i++;
  translateRelType(r, rel);
  assert(i < str.length() && str[i] == ' ');//FIXME:must be an exception;
  i++;
  //Here we expect package version;
  rel.ver.erase();
  while(i < str.length())
    rel.ver += str[i++];
}

static void parsePkgFileSection(const StringList& sect, PkgFile& pkgFile)
{
  pkgFile.source = 0;
  assert(!sect.empty());
  StringList::const_iterator it = sect.begin();
  pkgFile.fileName = *it;
  assert(pkgFile.fileName.length() > 2);//FIXME:must be exception;
  for(std::string::size_type i = 1;i < pkgFile.fileName.length();i++)
    pkgFile.fileName[i - 1] = pkgFile.fileName[i];
  pkgFile.fileName.resize(pkgFile.fileName.size() - 2);
  it++;
  assert(it != sect.end());//FIXME:must be exception;
  for(;it != sect.end();it++)
    {
      const std::string& line = *it;
      assert(!line.empty());
      std::string tail;
      //Parsing only name, epoch, version, release and all relations;
      if (stringBegins(line, "n=", tail))
	{
	pkgFile.name = tail;
	continue;
	}
      if (stringBegins(line, "e=", tail))
	{
	  std::istringstream ss(tail);
	  if (!(ss >> pkgFile.epoch))
	    {
	      assert(0);//FIXME:must be an exception;
	    }
	  continue;
	}
      if (stringBegins(line, "v=", tail))
	{
	  pkgFile.version = tail;
	  continue;
	}
      if (stringBegins(line, "r=", tail))
	{
	  pkgFile.release = tail;
	  continue;
	}
      if (stringBegins(line, "r:", tail))
	{
	  NamedPkgRel r;
	  parsePkgRel(tail, r);
	  pkgFile.requires.push_back(r);
	  continue;
	}
      if (stringBegins(line, "c:", tail))
	{
	  NamedPkgRel r;
	  parsePkgRel(tail, r);
	  pkgFile.conflicts.push_back(r);
	  continue;
	}
      if (stringBegins(line, "p:", tail))
	{
	  NamedPkgRel r;
	  parsePkgRel(tail, r);
	  pkgFile.provides.push_back(r);
	  continue;
	}
      if (stringBegins(line, "o:", tail))
	{
	  NamedPkgRel r;
	  parsePkgRel(tail, r);
	  pkgFile.obsoletes.push_back(r);
	  continue;
	}
    }
}

void RepoIndexTextFormatReader::openPackagesFile()
{
  assert(m_reader.get() == NULL);
  m_noMoreData = 0;
  if (m_compressionType == RepoIndexParams::CompressionTypeNone)
    m_reader = createTextFileReader(TextFileStd, Directory::mixNameComponents(m_dir, REPO_INDEX_PACKAGES_DATA_FILE)); else
  if (m_compressionType == RepoIndexParams::CompressionTypeGzip)
    m_reader = createTextFileReader(TextFileGZip, Directory::mixNameComponents(m_dir, std::string(REPO_INDEX_PACKAGES_DATA_FILE) + ".gz")); else 
    {
      assert(0);
    }
}

void RepoIndexTextFormatReader::openSourcePackagesFile()
{
  assert(0);
  //FIXME:
}

void RepoIndexTextFormatReader::openProvidesFile()
{
  m_noMoreData = 0;
  if (m_compressionType == RepoIndexParams::CompressionTypeNone)
    m_reader = createTextFileReader(TextFileStd, Directory::mixNameComponents(m_dir, REPO_INDEX_PROVIDES_DATA_FILE)); else
  if (m_compressionType == RepoIndexParams::CompressionTypeGzip)
    m_reader = createTextFileReader(TextFileGZip, Directory::mixNameComponents(m_dir, std::string(REPO_INDEX_PROVIDES_DATA_FILE) + ".gz")); else 
    {
      assert(0);
    }
}

bool RepoIndexTextFormatReader::readPackage(PkgFile& pkgFile)
{
  pkgFile.fileName.erase();
  pkgFile.name.erase();
  pkgFile.epoch = 0;
  pkgFile.version.erase();
  pkgFile.release.erase();
  //FIXME:other string fields;
  pkgFile.requires.clear();
  pkgFile.provides.clear();
  pkgFile.conflicts.clear();
  pkgFile.obsoletes.clear();
  pkgFile.changeLog.clear();
  if (m_noMoreData || m_reader.get() == NULL)
    return 0;
  std::string line;
  StringList section;
  if (m_lastSectionHeader.empty())
    {
      //There is no previously recognized section header, so we must find it;
      while(m_reader->readLine(line))
	if (!line.empty())
	  break;
      if (line.empty())//It is just empty file;
	{
	  m_noMoreData = 1;
	  return 0;
	}
      assert(line.length() > 2 && line[0] == '[' && line[line.length() - 1] == ']');//FIXME:must be an exception;
      section.push_back(line);
      //OK, we have found section header and can process it content;
    } else
    section.push_back(m_lastSectionHeader);//Adding section header from previous reading attempt;
  m_noMoreData = 1;
  while(m_reader->readLine(line))
    {
      if (line.length() > 2 && line[0] == '[' && line[line.length() - 1] == ']')
	{
	  m_lastSectionHeader = line;
	  m_noMoreData = 0;
	  break;
	}
      if (!line.empty())
	section.push_back(line);
    }
  parsePkgFileSection(section, pkgFile);
  return 1;
}

bool RepoIndexTextFormatReader::readProvides(std::string& provideName, StringVector& providers)
{
  provideName.erase();
  providers.clear();
  if (m_reader.get() == NULL || m_noMoreData)
    return 0;
  std::string line;
  //Sections are delimited by an empty line, searching first non-empty line;
      while(m_reader->readLine(line))
	if (!line.empty())
	  break;
      if (line.empty())//No more data;
	{
	  m_noMoreData = 1;
	  return 0;
	}
      assert(line.length() > 2 && line[0] == '[' && line[line.length() - 1] == ']');//FIXME:must be an exception;
  for(std::string::size_type i = 1;i < line.length();i++)
    line[i - 1] = line[i];
  line.resize(line.size() - 2);
  provideName = line;
      //OK, we have found section header and can process it content;
  while(m_reader->readLine(line))
    {
      if (line.empty())
	return 1;
      providers.push_back(line);
    }
  m_noMoreData = 1;
  return 1;
}
