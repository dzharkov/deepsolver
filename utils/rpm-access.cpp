RPMFileHandler::RPMFileHandler(string File)
{
   ID = File;
   FD = Fopen(File.c_str(), "r");
   if (FD == NULL)
   {
      /*
      _error->Error(_("could not open RPM package list file %s: %s"),
		    File.c_str(), rpmErrorString());
      */
      return;
   }
   iSize = fdSize(FD);
}


bool RPMSingleFileHandler::Skip()
{
   if (FD == NULL)
      return false;
   if (HeaderP != NULL) {
      headerFree(HeaderP);
      HeaderP = NULL;
      return false;
   }
#if RPM_VERSION >= 0x040100
   rpmts TS = rpmtsCreate();
   rpmtsSetVSFlags(TS, (rpmVSFlags_e)-1);
   int rc = rpmReadPackageFile(TS, FD, sFilePath.c_str(), &HeaderP);
   if (rc != RPMRC_OK && rc != RPMRC_NOTTRUSTED && rc != RPMRC_NOKEY) {
      _error->Error(_("Failed reading file %s"), sFilePath.c_str());
      HeaderP = NULL;
   }
   rpmtsFree(TS);
#else
   int rc = rpmReadPackageHeader(FD, &HeaderP, 0, NULL, NULL);
   if (rc) {
      _error->Error(_("Failed reading file %s"), sFilePath.c_str());
      HeaderP = NULL;
   }
#endif
   return (HeaderP != NULL);
}


string RPMFileHandler::FileName()
{
   char *str;
   int_32 count, type;
   assert(HeaderP != NULL);
   int rc = headerGetEntry(HeaderP, CRPMTAG_FILENAME,
			   &type, (void**)&str, &count);
   assert(rc != 0);
   return str;
}

