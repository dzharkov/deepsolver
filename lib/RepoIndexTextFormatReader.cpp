

#include"depsolver.h"
#include"RepoIndexTextFormatReader.h"

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
