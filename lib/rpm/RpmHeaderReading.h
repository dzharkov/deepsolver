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

#ifndef DEEPSOLVER_RPM_HEADER_READING_H
#define DEEPSOLVEr_RPM_HEADER_READING_H

#include"Pkg.h"
#include"RpmException.h"
#include<rpm/rpmlib.h>

void rpmFillMainData(Header& header, PkgBase& pkg);
void rpmFillProvides(Header& header, NamedPkgRelVector& v);
void rpmFillConflicts(Header& header, NamedPkgRelVector& v);
void rpmFillObsoletes(Header& header, NamedPkgRelVector& v);
void rpmFillRequires(Header& header, NamedPkgRelVector& v);
void rpmFillChangeLog(Header& header, ChangeLog& changeLog);
void rpmFillFileList(Header& header, StringList& v);
//Throws RpmException if required tag does not exist;
void rpmGetStringTagValue(Header& header, int_32 tag, std::string& value);
//Does nothing if required tag does not exist; 
void rpmGetStringTagValueRelaxed(Header& header, int_32 tag, std::string& value);
//Does nothing if required tag does not exist; 
void rpmGetInt32TagValueRelaxed(Header& header, int_32 tag, int_32& value);

#endif //DEEPSOLVER_RPM_HEADER_READING_H;
