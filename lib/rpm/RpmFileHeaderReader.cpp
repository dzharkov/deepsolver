/*
   Copyright 2011-2012 ALT Linux
   Copyright 2011-2012 Michael Pozhidaev

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

//Written with librpm-4.0.4-alt100.29;

//FIXME:requires just for install/removing
//FIXME:package size and required disk space;
//FIXME:requires flags for package install and removing;

#include"deepsolver.h"
#include"RpmFileHeaderReader.h"
#include"rpmHeader.h"

void RpmFileHeaderReader::load(const std::string& fileName)
{
  assert(m_fd == NULL);
  assert(m_header == NULL);
  m_fd = Fopen(fileName.c_str(), "r");
  if (m_fd == NULL)
    throw RpmException("Could not open rpm file \'" + fileName + "\' for header reading");
  const rpmRC rc = rpmReadPackageHeader(m_fd, &m_header, 0, NULL, NULL);
  if (rc != RPMRC_OK || m_header == NULL)
    {
      if (m_header)
	headerFree(m_header);
      Fclose(m_fd);
      m_fd = NULL;
      m_header = NULL;
      throw RpmException("Could not read header from rpm file \'" + fileName + "\'");
    }
  m_fileName = fileName;
}

void RpmFileHeaderReader::close()
{
  if (m_header != NULL)
    headerFree(m_header);
  if (m_fd != NULL)
    Fclose(m_fd);
  m_header = NULL;
  m_fd = NULL;
}

void RpmFileHeaderReader::fillMainData(PkgFileBase& pkg)
{
  rpmFillMainData(m_header, pkg);
}

void RpmFileHeaderReader::fillProvides(NamedPkgRelVector& v)
{
  rpmFillProvides(m_header, v);
}

void RpmFileHeaderReader::fillConflicts(NamedPkgRelVector& v)
{
  rpmFillConflicts(m_header, v);
}

void RpmFileHeaderReader::fillObsoletes(NamedPkgRelVector& v)
{
  rpmFillObsoletes(m_header, v);
}

void RpmFileHeaderReader::fillRequires(NamedPkgRelVector& v)
{
  rpmFillRequires(m_header, v);
}

void RpmFileHeaderReader::fillChangeLog(ChangeLog& changeLog)
{
  rpmFillChangeLog(m_header, changeLog);
}

void RpmFileHeaderReader::fillFileList(StringVector& v)
{
  rpmFillFileList(m_header, v);
}
