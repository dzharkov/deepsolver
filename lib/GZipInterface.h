
#ifndef DEPSOLVER_GZIP_INTERFACE_H
#define DEPSOLVER_GZIP_INTERFACE_H

#include"DepsolverException.h"

class GZipException: public DepsolverException
{
public:
  GZipException(const std::string& msg)
    : m_msg(msg) {}

  virtual ~GZipException() {}

public:
  std::string getType() const
  {
    return "gzip";
  }

  std::string getMessage() const
  {
    return m_msg;
  }

private:
  const std::string m_msg;
}; //class GZipException;

class GZipOutputFile
{
public:
  GZipOutputFile()
    : m_fd(-1),
      m_gzfile(NULL) {}

  ~GZipOutputFile()
  {
    close();
  }

public:
  void open(const std::string& fileName);
  void write(const void* buf, size_t bufSize);
  void close();

private:
  int m_fd;
  void* m_gzfile;
  std::string m_fileName;
}; //class GZipOutputFile;

class GZipInputFile
{
public:
  GZipInputFile()
    : m_fd(-1),
      m_gzfile(NULL) {}

  ~GZipInputFile()
  {
    close();
  }

public:
  void open(const std::string& fileName);
  //The method below returns less data than requested block size in case of end of file;
  size_t read(void* buf, size_t bufSize);
  void close();

private:
  int m_fd;
  void* m_gzfile;
  std::string m_fileName;
}; //class GZipOutputFile;

#endif //DEPSOLVER_GZIP_INTERFACE_H;
