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

//FIXME:This file needs more work on proper exception throwing instead of currently used asserts;

#include"deepsolver.h"
#include"TextFormatReader.h"

void TextFormatReader::openFile(const std::string& fileName, int textFileType)
{
  assert(m_reader.get() == NULL);
  m_noMoreData = 0;
  m_reader = createTextFileReader(textFileType, fileName);
  m_fileName = fileName;
  m_lineNumber = 0;
}

void TextFormatReader::close()
{
  assert(m_reader.get() != NULL);
  delete m_reader.release();
  m_fileName.erase();
}

bool TextFormatReader::readPackage(PkgFile& pkgFile)
{
  pkgFile = PkgFile();
  if (m_noMoreData || m_reader.get() == NULL)
    return 0;
  std::string line;
  StringList section;
  if (m_lastSectionHeader.empty())
    {
      //There is no previously recognized section header, so we must find it;
      while(readLine(line))
	if (!line.empty())
	  break;
      if (line.empty())//It is just empty file;
	{
	  m_noMoreData = 1;
	  return 0;
	}
      if(line.length() <= 2 || line[0] != '[' || line[line.length() - 1] != ']')
	throw TextFormatReaderException(TextFormatReaderErrorInvalidSectionHeader, m_fileName, m_lineNumber, m_line);
      section.push_back(line);
      //OK, we have found section header and can process it content;
    } else
    section.push_back(m_lastSectionHeader);//Adding section header from previous reading attempt;
  m_noMoreData = 1;
  while(readLine(line))
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

bool TextFormatReader::readProvides(std::string& provideName, StringVector& providers)
{
  provideName.erase();
  providers.clear();
  if (m_reader.get() == NULL || m_noMoreData)
    return 0;
  std::string line;
  //Sections are delimited by an empty line, searching first non-empty line;
      while(readLine(line))
	if (!line.empty())
	  break;
      if (line.empty())//No more data;
	{
	  m_noMoreData = 1;
	  return 0;
	}
      if (line.length() <= 2 || line[0] != '[' || line[line.length() - 1] != ']')
	throw TextFormatReaderException(TextFormatReaderErrorInvalidSectionHeader, m_fileName, m_lineNumber, m_line);
  for(std::string::size_type i = 1;i < line.length();i++)
    line[i - 1] = line[i];
  line.resize(line.size() - 2);
  provideName = line;
      //OK, we have found section header and can process it content;
  while(readLine(line))
    {
      if (line.empty())
	return 1;
      providers.push_back(line);
    }
  m_noMoreData = 1;
  return 1;
}

void TextFormatReader::translateRelType(const std::string& str, NamedPkgRel& rel)
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

void TextFormatReader::parsePkgRel(const std::string& str, NamedPkgRel& rel)
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

void TextFormatReader::parsePkgFileSection(const StringList& sect, PkgFile& pkgFile)
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
      //Parsing only name, epoch, version, release, buildtime and all relations;
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
      if (stringBegins(line, "btime=", tail))
	{
	  std::istringstream ss(tail);
	  if (!(ss >> pkgFile.buildTime))
	    {
	      assert(0);//FIXME:must be an exception;
	    }
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

bool TextFormatReader::readLine(std::string& line)
{
  assert(m_reader.get() != NULL);
  if (!m_reader->readLine(line))
    return 0;
  m_lineNumber++;
  m_line = line;
  return 1;
}
