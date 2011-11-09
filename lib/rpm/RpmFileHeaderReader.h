
#ifndef FIXME_RPM_FILE_HEADER_READER_H
#define FIXME_RPM_FILE_HEADER_READER_H

#include"RpmException.h"
#include<rpm/rpmlib.h>

class RpmFileHeaderReader
{
public:
  RpmFileHeaderReader()
    : m_fd(NULL), m_header(NULL) {}

  ~RpmFileHeaderReader{}
  {
    close();
  }

public:
  void load(const std::string& fileName);
  void close();

private:
  FD_t m_fd;
  Header m_header;
}; //class RpmFileHeaderReader;

#endif //FIXME_RPM_FILE_HEADER_READER_H;
