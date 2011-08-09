
#include<assert.h>
#include<string>
#include<iostream>
#include<rpm/rpmlib.h>

bool processFile(const std::string& fileName)
{
  FD_t fd = Fopen(fileName.c_str(), "r");
  assert(fd != NULL);
  if (fd == NULL)
    return 0;
  Header h;
  /*
  rpmts ts = rpmtsCreate();
  rpmtsSetVSFlags(TS, (rpmVSFlags_e)-1);
  int rc = rpmReadPackageFile(TS, FD, sFilePath.c_str(), &HeaderP);
   if (rc != RPMRC_OK && rc != RPMRC_NOTTRUSTED && rc != RPMRC_NOKEY) {
      _error->Error(_("Failed reading file %s"), sFilePath.c_str());
      HeaderP = NULL;
   }
   rpmtsFree(TS);
  */
  int rc = rpmReadPackageHeader(fd, &h, 0, NULL, NULL);
  assert(!rs);
  if (rc) 
    return 0;
  char* str;
  int_32 count, type;
  int rc = headerGetEntry(h, CRPMTAG_FILENAME, &type, (void**)&str, &count);
  assert(rc != 0);
  std::cout << str << std::endl;j
				  return 1;
}

int main(int argc, char* argv[])
{
  assert(argc >= 2);
  processFile(argv[1]);
  return 0;
}
