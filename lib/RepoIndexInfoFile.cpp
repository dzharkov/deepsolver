
#include"basic-header.h"//FIXME:
#include"RepoIndexInfoFile.h"

bool RepoIndexInfoFile::read(const std::string& fileName, StringList& errorMessages, StringList& warningMessages)
{
  errorMessages.clear();
  warningMessages.clear();
}

bool RepoIndexInfoFile::write(const std::string& fileName, StringList& errorMessages, StringList& warningMessages)
{
  assert(hasNonSpaces(m_repoName));
  assert(hasNonSpaces(m_compressionType));
  assert(hasNonSpaces(m_formatType));
  errorMessages.clear();
  warningMessages.clear();
  return 0;
}
