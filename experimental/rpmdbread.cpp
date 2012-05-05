
#include<iostream>
#include<assert.h>
#include <stdio.h>                                                                                                                                             
#include <stdlib.h>                                                                                                                                            
#include <string.h>                                                                                                                                            
#include <fcntl.h>                                                                                                                                             
#include <unistd.h>                                                                                                                                            
#include <rpm/rpmlib.h>                                                                                                                                        
#include <rpm/header.h>                                                                                                                                        
#include <rpm/rpmdb.h>                                                                                                                                         

int                                                                                                                                                            main(int argc, char *argv[])                                                                                                                                   
{                                                                                                                                                              
  const int rpmdbFd = open("/var/lib/rpm/Packages", O_RDONLY);
  assert(rpmdbFd > 0);
  struct stat st;
  fstat(rpmdbFd, &st);
  readahead(rpmdbFd, 0, st.st_size);
  close(rpmdbFd);
  rpmdbMatchIterator mi;                                                                                                                                     
  int type, count;                                                                                                                                           
  char *name;                                                                                                                                                
  rpmdb db;                                                                                                                                                  
  Header h;                                                                                                                                                  
  rpmReadConfigFiles( NULL, NULL );                                                                                                                          
  assert(rpmdbOpen( "", &db, O_RDONLY, 0644 ) == 0);
  mi = rpmdbInitIterator(db, RPMDBI_PACKAGES, NULL, 0);                                                                                                      
  while ((h = rpmdbNextIterator(mi)))
    {                                                                                                                      
      headerGetEntry(h, RPMTAG_NAME, &type, (void **) &name, &count);                                                                                        
      printf("%s-", name);                                                                                                                                  
      headerGetEntry(h, RPMTAG_VERSION, &type, (void **) &name, &count);                                                                                        
      printf("%s-", name);                                                                                                                                  
      headerGetEntry(h, RPMTAG_RELEASE, &type, (void **) &name, &count);                                                                                        
      printf("%s\n", name);                                                                                                                                  
    }                                                                                                                                                          
  rpmdbFreeIterator(mi);                                                                                                                                     
  rpmdbClose(db);                                                                                                                                            
                                                                                  exit(0);                                                                                                                                                   
}                                                                                                                                                              
