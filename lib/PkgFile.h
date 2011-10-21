
#ifndef FIXME_PKG_FILE_H
#define FIXME_PKG_FILE_H

class PkgFile: public Pkg
{
public:
  PkgFile() {}
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

protected:
  std::string m_fileName;
}; //class PkgFile;

#endif //FIXME_PKG_FILE_H;
