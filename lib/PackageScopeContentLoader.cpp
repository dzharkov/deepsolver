
#include"deepsolver.h"
#include"PackageScopeContentLoader.h"

#define BLOCK_SIZE 512

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
      const size_t toRead = (bufSize - count) > BLOCK_SIZE?BLOCK_SIZE:(bufSize - count);
      s.read(buf + count, toRead);
      assert(s);//FIXME:must be an exception;
      count += toRead;
    }
  assert(count == bufSize);
}

void PackageScopeContentLoader::loadFromFile(const std::string& fileName)
{
  logMsg(LOG_DEBUG, "Starting reading from binary file \'%s\'", fileName.c_str());
  assert(!fileName.empty());
  assert(m_names.empty());
  assert(m_pkgInfoVector.empty());
  assert(m_relInfoVector.empty());
  assert(m_provideMap.empty());
  assert(m_stringBuf == NULL);
  std::ifstream s(fileName.c_str());
  assert(s.is_open());//FIXME:error checking;
  //Reading numbers of records;
  const size_t numStringValues = readSizeValue(s);
  logMsg(LOG_DEBUG, "%zu string table entries", numStringValues);
  const size_t stringBufSize = readSizeValue(s);
  logMsg(LOG_DEBUG, "%zu bytes in all string constants with trailing zeroes", stringBufSize);
  const size_t nameCount = readSizeValue(s);
  logMsg(LOG_DEBUG, "%zu package names", nameCount);
  const size_t namesBufSize = readSizeValue(s);
  logMsg(LOG_DEBUG, "%zu bytes in all package names with trailing zeroes", namesBufSize);
  m_pkgInfoVector.resize(readSizeValue(s));
  logMsg(LOG_DEBUG, "%zu packages", m_pkgInfoVector.size());
  m_relInfoVector.resize(readSizeValue(s));
  logMsg(LOG_DEBUG, "%zu package relations", m_relInfoVector.size());
  m_provideMap.resize(readSizeValue(s));
  logMsg(LOG_DEBUG, "%zu provide map items", m_provideMap.size());
  //Reading strings bounds;
  SizeVector stringBounds;
  stringBounds.resize(numStringValues);
  for(size_t i = 0;i < numStringValues;i++)
    {
      stringBounds[i] = readSizeValue(s);
      assert(stringBounds[i] < stringBufSize);
    }
  assert(stringBounds.empty() || stringBounds[0] == 0);
  //Reading all version and release strings;
  m_stringBuf = new char[stringBufSize];
  readBuf(s, m_stringBuf, stringBufSize );
  //Reading names of packages and provides;
  m_names.reserve(nameCount);
    readNames(s, namesBufSize);
  assert(m_names.size() == nameCount);
  for(StringVector::size_type i = 0;i < m_names.size();i++)
    m_namesToId.insert(NameToPackageIdMap::value_type(m_names[i], i));
  //Reading package list;
  for(PkgInfoVector::size_type i = 0;i < m_pkgInfoVector.size();i++)
    {
      PkgInfo& info = m_pkgInfoVector[i];
      info.pkgId = readSizeValue(s);
      info.epoch = readUnsignedShortValue(s);
      const size_t verId = readSizeValue(s);
      assert(verId < stringBounds.size());//FIXME:must be an exception;
      info.ver = m_stringBuf + stringBounds[verId];
      const size_t releaseId = readSizeValue(s);
      assert(releaseId < stringBounds.size());//FIXME:must be an exception;
      info.release = m_stringBuf + stringBounds[releaseId];
      info.buildTime = readSizeValue(s);
      info.requiresPos = readSizeValue(s);
      info.requiresCount = readSizeValue(s);
      info.providesPos = readSizeValue(s);
      info.providesCount = readSizeValue(s);
      info.conflictsPos = readSizeValue(s);
      info.conflictsCount = readSizeValue(s);
      info.obsoletesPos = readSizeValue(s);
      info.obsoletesCount = readSizeValue(s);
    }
  //Reading package relations;
  for(RelInfoVector::size_type i = 0;i < m_relInfoVector.size();i++)
    {
      RelInfo& info = m_relInfoVector[i];
      info.pkgId = readSizeValue(s);
      info.type = readCharValue(s);
      const size_t verId = readSizeValue(s);
      if (verId != (size_t)-1)
	{
	  assert(verId < stringBounds.size());//FIXME:must be an exception;
	  info.ver = m_stringBuf + stringBounds[verId];
	}
    }
  //Reading provide map;
  for(ProvideMapItemVector::size_type i = 0;i < m_provideMap.size();i++)
    {
      m_provideMap[i].provideId = readSizeValue(s);
      m_provideMap[i].pkgId = readSizeValue(s);
    }
}

void PackageScopeContentLoader::readNames(std::ifstream& s, size_t namesBufSize)
{
  size_t count = 0;
  char buf[BLOCK_SIZE];
  std::string prev;
  while(count < namesBufSize)
    {
      const size_t toRead = (namesBufSize - count) > BLOCK_SIZE?BLOCK_SIZE:(namesBufSize - count);
      s.read(buf, toRead);
  assert(s);//FIXME:must be an exception;
  size_t i = 0;
  if (!prev.empty())//We have chunk of incomplete string from previous reading attempt;
    {
      while(i < toRead && buf[i] != '\0')
	i++;
      if (i >= toRead)
	{
	  for(size_t k = 0;k < toRead;k++) 
	    prev += buf[k];
	  continue;
	}
      //OK, we have at least one '\0' in buf, so can safely add the string until '\0' to 'prev' variable;
      prev += buf;
      m_names.push_back(prev);
	      prev.erase();
	      //OK, incomplete string was successfully processed;
	      i++;
    } //If we have incomplete string chunk;
  size_t fromPos = i;
      for(;i < toRead;i++)
	{
	  assert(fromPos < toRead && i < toRead);
	  if (buf[i] != '\0')
	    continue;
	      m_names.push_back(buf + fromPos);
	      fromPos = i + 1;
	} //for(buf);
      assert(fromPos <= i);
      assert(prev.empty());
      if (fromPos < i)
	for(size_t k = fromPos;k < i;k++)
	  {
	    assert(k < toRead);
	    prev += buf[k];
	  }
      count += toRead;
    }
  assert(count == namesBufSize);
  assert(prev.empty());//FIXME:must be an exception;
}
