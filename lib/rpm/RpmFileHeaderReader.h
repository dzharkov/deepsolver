
#ifndef FIXME_RPM_FILE_HEADER_READER_H
#define FIXME_RPM_FILE_HEADER_READER_H

#include"PkgFile.h"
#include"NamedPkgRel.h"
#include"RpmException.h"
#include<rpm/rpmlib.h>

class RpmFileHeaderReader
{
public:
  RpmFileHeaderReader()
    : m_fd(NULL), m_header(NULL) {}

  ~RpmFileHeaderReader()
  {
    close();
  }

public:
  void load(const std::string& fileName);
  void close();
  void fillMainData(PkgFile& pkg);
  void fillProvides(NamedPkgRelList& v);
  void fillConflicts(NamedPkgRelList& v);
  void fillObsoletes(NamedPkgRelList& v);
  void fillRequires(NamedPkgRelList& v);
  void fillFileList(StringList& v);

private:
  //Throws RpmException if required tag does not exist;
  void getStringTagValue(int_32 tag, std::string& value);
  //Does nothing if required tag does not exist; 
  void getStringTagValueRelaxed(int_32 tag, std::string& value);
  //Does nothing if required tag does not exist; 
  void getInt32TagValueRelaxed(int_32 tag, int_32& value);

private:
  FD_t m_fd;
  Header m_header;
  std::string m_fileName;
}; //class RpmFileHeaderReader;

#endif //FIXME_RPM_FILE_HEADER_READER_H;
