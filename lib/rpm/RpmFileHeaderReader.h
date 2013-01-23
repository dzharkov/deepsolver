/*
   Copyright 2011-2013 ALT Linux
   Copyright 2011-2013 Michael Pozhidaev

   This file is part of the Deepsolver.

   Deepsolver is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   Deepsolver is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
*/

#ifndef DEEPSOLVER_RPM_FILE_HEADER_READER_H
#define DEEPSOLVEr_RPM_FILE_HEADER_READER_H

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
  void fillFileList(StringVector& v);

private:
  FD_t m_fd;
  Header m_header;
  std::string m_fileName;
}; //class RpmFileHeaderReader;

#endif //DEEPSOLVER_RPM_FILE_HEADER_READER_H;
