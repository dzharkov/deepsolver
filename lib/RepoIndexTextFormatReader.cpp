
#include"depsolver.h"
#include"RepoIndexTextFormatReader.h"

static void parsePkgFileSection(const StringList& sect, PkgFile& pkgFile)
{
  pkgFile.source = 0;
  assert(!sect.empty());
  StringList::const_iterator it = sect.begin();
  pkgFile.fileName = *it;
  assert(pkgFile.fileName.length() > 2);//FIXME:must be exception;
  for(std:;string::size_type i = 1;i < pkgFile.fileName.length();i++)
    pkgFile.fileName[i - 1] = pkgFile.fileName[i];
  pkgFile.fileName.resize(pkgFile.size() - 2);
  it++;
  assert(it != sect.end());//FIXME:must be exception;
  while(it != sect.end())
    {
      const std::string& line = *it;
    }


void RepoIndexTextFormatReader::openRpms();
{
  assert(m_reader.get() == NULL);
  m_noMoreData = 0;
  //FIXME:
}

void RepoIndexTextFormatReader::openSrpms()
{
  assert(m_reader.get() == NULL);
  m_noMoreData = 0;
  //FIXME:
}

void RepoIndexTextFormatReader::openProvides()
{
  assert(m_reader.get() == NULL);
  m_noMoreData = 0;
  //FIXME:
}

bool RepoIndexTextFormatReader::readPackage(PkgFile& pkgFile)
{
  if (m_noMoreData || m_reader.get() == NULL)
    return 0;
  StringList section;
  section.push_back(m_lastSectionHeader);
  std::string line;
  m_noMoreData = 1;
  while(m_reader.->readLine(line))
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

bool RepoIndexTextFormatReader::readProvides(std::string& provideName, StringList& providers)
{
  if (m_reader.get() == NULL)
    return 0;
}
