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

void RpmFileHeaderReader::load(const std::string& fileName)
{
  assert(m_fd == NULL);
  assert(m_header == NULL);
  m_fd = Fopen(fileName.c_str(), "r");
  if (m_fd == NULL)
    RPM_STOP("Could not open rpm file \'" + fileName + "\' for header reading");
  const rpmRC rc = rpmReadPackageHeader(m_fd, &m_header, 0, NULL, NULL);
  if (rc != RPMRC_OK || m_header == NULL)
    {
      if (m_header)
	headerFree(m_header);
      Fclose(m_fd);
      m_fd = NULL;
      m_header = NULL;
      RPM_STOP("Could not read header from rpm file \'" + fileName + "\'");
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
  //FIXME:
}

void RpmFileHeaderReader::fillProvides(NamedPkgRelVector& v)
{
  //FIXME:
}

void RpmFileHeaderReader::fillConflicts(NamedPkgRelVector& v)
{
  //FIXME:
}

void RpmFileHeaderReader::fillObsoletes(NamedPkgRelVector& v)
{
  //FIXME:
}

void RpmFileHeaderReader::fillRequires(NamedPkgRelVector& v)
{
  //FIXME:
}

void RpmFileHeaderReader::fillChangeLog(ChangeLog& changeLog)
{
  //FIXME:
}

void RpmFileHeaderReader::fillFileList(StringList& v)
{
  //FIXME:
}

void readRpmPkgFile(const std::string& fileName, PkgFile& pkgFile, StringList& fileList)
{
  RpmFileHeaderReader reader;
  reader.load(fileName);
  reader.fillMainData(pkgFile);
  reader.fillProvides(pkgFile.provides);
  reader.fillConflicts(pkgFile.conflicts);
  reader.fillObsoletes(pkgFile.obsoletes);
  reader.fillRequires(pkgFile.requires);
  reader.fillChangeLog(pkgFile.changeLog);
  reader.fillFileList(fileList);
  reader.close();
  pkgFile.fileName = fileName;
}

