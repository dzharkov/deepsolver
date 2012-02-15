
#ifndef DEEPSOLVER_RPM_FILE_HEADER_READER_H
#define DEEPSOLVEr_RPM_FILE_HEADER_READER_H

#include"Pkg.h"
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
  void fillMainData(PkgFileBase& pkg);
  void fillProvides(NamedPkgRelVector& v);
  void fillConflicts(NamedPkgRelVector& v);
  void fillObsoletes(NamedPkgRelVector& v);
  void fillRequires(NamedPkgRelVector& v);
  void fillChangeLog(ChangeLog& changeLog);
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

#endif //DEEPSOLVER_RPM_FILE_HEADER_READER_H;
