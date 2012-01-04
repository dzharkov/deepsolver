

#include"depsolver.h"
#include"RepoIndexTextFormatReader.h"

void RepoIndexTextFormatReader::openRpms();
{
  assert(m_reader.get() == NULL);
  //FIXME:
}

void RepoIndexTextFormatReader::openSrpms()
{
  assert(m_reader.get() == NULL);
  //FIXME:
}

void RepoIndexTextFormatReader::openProvides()
{
  assert(m_reader.get() == NULL);
  //FIXME:
}

bool RepoIndexTextFormatReader::readPackage(PkgFile& pkgFile)
{
  if (m_reader.get() == NULL)
    return 0;
}

bool RepoIndexTextFormatReader::readProvides(std::string& provideName, StringList& providers)
{
  if (m_reader.get() == NULL)
    return 0;
}
