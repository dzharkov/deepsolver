
#ifndef FIXME_REPO_INDEX_INFO_FILE_H
#define FIXME_REPO_INDEX_INFO_FILE_H

class RepoIndexInfoFile
{
public:
  RepoIndexInfoFile() {}

public:
  bool read(const std::string& fileName, std::string& errorMessage, StringList& warningMessages);
  bool write(const std::string& fileName, std::string& errorMessage, StringList& warningMessages);

  std::string getRepoName() const
  {
    return m_repoName;
  }

  void setRepoName(const std::string repoName)
  {
    m_repoName = repoName;
  }

  std::string getCompressionType() const 
  {
    return m_compressionType;
  }

  void setCompressionType(const std::string compressionType)
  {
    m_compressionType = compressionType;
  }

  std::string getFormatType() const
  {
    return m_formatType;
  }

  void setFormatType(const std::string& formatType)
  {
    m_formatType = formatType;
  }

private:
  std::string m_repoName;
  std::string m_compressionType;
  std::string m_formatType;
}; //class RepoIndexInfoFile;

#endif //FIXME_REPO_INDEX_INFO_FILE_H;
