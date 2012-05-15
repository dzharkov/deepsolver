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

#include"deepsolver.h"
#include <rpm/rpmlib.h>                                                                                                                                        
#include <rpm/header.h>                                                                                                                                        
#include <rpm/rpmdb.h>                                                                                                                                         

static bool alreadyReadConfigFiles = 0;

void Rpmdb::openEnum()
{
  if (!alreadyReadConfigFiles)
    {
      rpmReadConfigFiles( NULL, NULL );                                                                                                                          
      alreadyReadConfigFiles = 1;
    }
  if (rpmdbOpen( "", &m_db, O_RDONLY, 0644 ) != 0)
    RPM_STOP("Could not open rpmdb");
  m_it = rpmdbInitIterator(m_db, RPMDBI_PACKAGES, NULL, 0);                                                                                                      
}

void Rpmdb::moveNext(Pkg& pkg)
{                                                                                                                                                              
  int type, count;
  char *str;
  Header h = rpmdbNextIterator(m_it);
  if (!h)
    return 0;
      headerGetEntry(h, RPMTAG_NAME, &type, (void **) &str, &count);                                                                                        
      assert(str != NULL);
      pkg.name = str;
      headerGetEntry(h, RPMTAG_VERSION, &type, (void **) &str, &count);                                                                                        
      assert(str != NULL);
      pkg.version = str;
      headerGetEntry(h, RPMTAG_RELEASE, &type, (void **) &str, &count);                                                                                        
      assert(str != NULL);
      pkg.release = str;
      return 1;
}                                                                                                                                                              

void Rpmdb::close()
{
  rpmdbFreeIterator(m_it);                                                                                                                                     
  rpmdbClose(m_db);                                                                                                                                            
}
