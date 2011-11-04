
#include"basic-header.h"//FIXME:
#include"RepoIndexInfoFile.h"

bool RepoIndexInfoFile::read(const std::string& fileName, std::string& errorMessage, StringList& warningMessages)
{
  errorMessage.erase();
  warningMessages.clear();
  return 0;
}

bool RepoIndexInfoFile::write(const std::string& fileName, std::string& errorMessage, StringList& warningMessages)
{
  assert(hasNonSpaces(m_repoName));
  assert(hasNonSpaces(m_compressionType));
  assert(hasNonSpaces(m_formatType));
  errorMessage.erase();
  warningMessages.clear();
  return 0;
}
