
#ifndef FIXME_PKG_FILE_H
#define FIXME_PKG_FILE_H

class PkgFile: public Pkg
{
public:
  PkgFile()
    : m_source(0)  {}
  virtual ~PkgFile() {}

public:
  std::string getFileName() const
  {
    return m_fileName;
  }

  void setFileName(const std::string& fileName)
  {
    m_fileName = fileName;
  }

  bool isSource() const
  {
    return m_source;
  }

  void setIsSource(bool isSource)
  {
    m_source = isSource;
  }

protected:
  std::string m_fileName;
  bool m_source;
}; //class PkgFile;

#endif //FIXME_PKG_FILE_H;
