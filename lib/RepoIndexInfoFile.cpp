
#include"basic-header.h"//FIXME:
#include"RepoIndexInfoFile.h"

bool RepoIndexINfoFile::read(const std::string& fileName, StringList& errorMessages, StringList& warningMessages)
{
  errorMessages.clear();
  warningMessages.clear();
}

bool RepoIndexINfoFile::write(const std::string& fileName, StringList& errorMessages, StringList& warningMessages)
{
  assert(hasNonSpaces(m_repoName));
  assert(hasNonSpaces(m_repoNamecompressionType
  assert(hasNonSpaces(m_formatType));
  errorMessages.clear();
  warningMessages.clear();
}
