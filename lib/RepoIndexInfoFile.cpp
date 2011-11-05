
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
  assert(hasNonSpaces(m_compressionType));
  assert(hasNonSpaces(m_formatType));
  errorMessage.erase();
  warningMessages.clear();
  std::ofstream f(fileName.c_str());
  if (!f)
    {
      errorMessage = "Could not create \'" + fileName + "\' for writing";
      return 0;
    }
  time_t t;
  time(&t);
  f << "# Repository index information file" << std::endl;
    f << "# Generated on " << ctime(&t) << std::endl;
  f << "# This file contains a set of options to control various parameters of" << std::endl;
  f << "# repository index structure. An empty lines are silently ignored. Any" << std::endl;
  f << "# line text after the character `#\' is skipped as a comment. " << std::endl;
  f << "# Character `\\\' should be used in the conjunction with the following character to" << std::endl;
  f << "# prevent special character processing." << std::endl;
  f << std::endl;
  f << "repo_name=" << m_repoName << std::endl;
  f << "format_type=" << m_formatType << std::endl;
  f << "compression_type=" << m_compressionType << std::endl;
  return 1;
}
