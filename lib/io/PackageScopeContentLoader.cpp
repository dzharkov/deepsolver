/*
   Copyright 2011-2013 ALT Linux
   Copyright 2011-2013 Michael Pozhidaev

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
#include"PackageScopeContentLoader.h"
#include"OperationCore.h"

#define IO_BLOCK_SIZE 512
#define THROW_INTERNAL_ERROR throw OperationException(OperationException::InternalIOProblem)

inline size_t readSizeValue(std::ifstream& s)
{
  size_t value = 0;
  s.read((char*)&value, sizeof(size_t));
  assert(s);//FIXME:must be an exception;
  return value;
}

inline unsigned short readUnsignedShortValue(std::ifstream& s)
{
  unsigned short value = 0;
  s.read((char*)&value, sizeof(unsigned short));
  assert(s);//FIXME:must be an exception;
  return value;
}

inline char readCharValue(std::ifstream& s)
{
  char value = 0;
  s.read(&value, sizeof(char));
  assert(s);//FIXME:must be an exception;
  return value;
}

static void   readBuf(std::ifstream& s, char* buf, size_t bufSize)
{
  assert(buf != NULL);
  size_t count = 0;
  while(count < bufSize)
    {
      const size_t toRead = (bufSize - count) > IO_BLOCK_SIZE?IO_BLOCK_SIZE:(bufSize - count);
      s.read(buf + count, toRead);
      assert(s);//FIXME:must be an exception;
      count += toRead;
    }
  assert(count == bufSize);
}

void PackageScopeContentLoader::loadFromFile(const std::string& fileName)
{
  logMsg(LOG_INFO, "Starting reading package scope content from binary file \'%s\'", fileName.c_str());
  assert(!fileName.empty());
  assert(m_c.names.empty());
  assert(m_c.pkgInfoVector.empty());
  assert(m_c.relInfoVector.empty());
  std::ifstream s(fileName.c_str());
  if (!s.is_open())
    {
      logMsg(LOG_ERR, "An error occurred opening \'%s\' for reading", fileName.c_str());
    THROW_INTERNAL_ERROR;
    }
  //Reading numbers of records;
  const size_t stringBufSize = readSizeValue(s);
  logMsg(LOG_DEBUG, "%zu bytes in all string constants with trailing zeroes", stringBufSize);
  const size_t nameCount = readSizeValue(s);
  logMsg(LOG_DEBUG, "%zu package names", nameCount);
  const size_t namesBufSize = readSizeValue(s);
  logMsg(LOG_DEBUG, "%zu bytes in all package names with trailing zeroes", namesBufSize);
  m_c.pkgInfoVector.resize(readSizeValue(s));
  logMsg(LOG_DEBUG, "%zu packages", m_c.pkgInfoVector.size());
  m_c.relInfoVector.resize(readSizeValue(s));
  logMsg(LOG_DEBUG, "%zu package relations", m_c.relInfoVector.size());
  const size_t controlValueHave = readSizeValue(s);
  const size_t controlValueShouldBe = stringBufSize + nameCount + namesBufSize + m_c.pkgInfoVector.size() + m_c.relInfoVector.size();
  if (controlValueShouldBe != controlValueHave)
    {
      logMsg(LOG_ERR, "Control value does not match: %zu have but %zu should be", controlValueHave, controlValueShouldBe);
      THROW_INTERNAL_ERROR;
    } else
    logMsg(LOG_DEBUG, "Control value correct (%zu)", controlValueHave);
  if (m_c.pkgInfoVector.empty())
    {
      logMsg(LOG_DEBUG, "There are no packages, leaving package scope empty");
      return;
    }
  //Reading all version and release strings;
  char* stringBuf = new char[stringBufSize];
  m_c.addStringToAutoRelease(stringBuf);
  readBuf(s, stringBuf, stringBufSize );
  //Reading names of packages and provides;
  m_c.names.reserve(nameCount);
  readNames(s, namesBufSize);
  if (!m_nameChunk.empty())
    {
      logMsg(LOG_ERR, "Non-empty name chunk after names reading \'%s\', already have %zu complete names", m_nameChunk.c_str(), m_c.names.size());
      THROW_INTERNAL_ERROR;
    }
  if(m_c.names.size() != nameCount)
    {
      logMsg(LOG_ERR, "Number of read names does not match expected value, read %zu but expected %zu", m_c.names.size(), nameCount);
      THROW_INTERNAL_ERROR;
    }
  //Reading package list;
  for(PackageScopeContent::PkgInfoVector::size_type i = 0;i < m_c.pkgInfoVector.size();i++)
    {
      PackageScopeContent::PkgInfo& info = m_c.pkgInfoVector[i];
      info.pkgId = readSizeValue(s);
      info.epoch = readUnsignedShortValue(s);
      const size_t verOffset = readSizeValue(s);
      assert(verOffset < stringBufSize);//FIXME:must be an exception;
      info.ver = stringBuf + verOffset;
      const size_t releaseOffset = readSizeValue(s);
      assert(releaseOffset < stringBufSize);//FIXME:must be an exception;
      info.release = stringBuf + releaseOffset;
      info.buildTime = readSizeValue(s);
      info.requiresPos = readSizeValue(s);
      info.requiresCount = readSizeValue(s);
      info.providesPos = readSizeValue(s);
      info.providesCount = readSizeValue(s);
      info.conflictsPos = readSizeValue(s);
      info.conflictsCount = readSizeValue(s);
      info.obsoletesPos = readSizeValue(s);
      info.obsoletesCount = readSizeValue(s);
      info.flags = PkgFlagAvailableByRepo;
    }
  //Reading package relations;
  for(PackageScopeContent::RelInfoVector::size_type i = 0;i < m_c.relInfoVector.size();i++)
    {
      PackageScopeContent::RelInfo& info = m_c.relInfoVector[i];
      info.pkgId = readSizeValue(s);
      info.type = readCharValue(s);
      const size_t verOffset = readSizeValue(s);
      if (verOffset != (size_t)-1)
	{
	  assert(verOffset < stringBufSize);//FIXME:must be an exception;
	  info.ver = stringBuf + verOffset;
	} else
	info.ver = NULL;
    }
}

void PackageScopeContentLoader::readNames(std::ifstream& s, size_t namesBufSize)
{
  logMsg(LOG_DEBUG, "Reading names, namesBufSize=%zu", namesBufSize);
  size_t count = 0;
  char buf[IO_BLOCK_SIZE + 1];
  buf[IO_BLOCK_SIZE] = '\0';
  while(count < namesBufSize)
    {
      const size_t toRead = (namesBufSize - count) > IO_BLOCK_SIZE?IO_BLOCK_SIZE:(namesBufSize - count);
      s.read(buf, toRead);
  assert(s);//FIXME:must be an exception;
  buf[toRead] = '\0';
  size_t fromPos = 0;
      for(size_t i = 0;i < toRead;i++)
	{
	  assert(fromPos < toRead && i < toRead && fromPos <= i);
	  if (buf[i] != '\0')
	    continue;
	  onNewName(buf + fromPos, 1);//1 means it is complete value;
	      fromPos = i + 1;
	} //for(buf);
      assert(fromPos <= toRead);
      if (fromPos < toRead)//We have incomplete value;
	onNewName(buf + fromPos, 0);//0 means it is incomplete value;
      count += toRead;
    }
  assert(count == namesBufSize);
}

void PackageScopeContentLoader::onNewName(const char* name, bool complete)
{
  assert(name);
  if (!complete)
    {
      m_nameChunk += name;
      return;
    }
  m_c.names.push_back(m_nameChunk + name);
  m_nameChunk.erase();
}
