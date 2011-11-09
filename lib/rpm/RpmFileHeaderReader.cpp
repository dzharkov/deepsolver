//Written with librpm-4.0.4-alt100.29;

#include"../basic-header.h"//FIXME:
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

void RpmFileHeaderReader::fillMainData(PkgFile& pkg)
{
  getStringTagValue(RPMTAG_NAME, pkg.name);
  //Epoch may be omitted, no need to check return code; 
  int32_t epoch = 0;
  getInt32TagValueRelaxed(RPMTAG_EPOCH, epoch);
  pkg.epoch = epoch;
  getStringTagValue(RPMTAG_VERSION, pkg.version);
  getStringTagValue(RPMTAG_RELEASE, pkg.release);
  getStringTagValue(RPMTAG_ARCH, pkg.arch);
  //URL may be omitted, no need to check return code; 
  pkg.url.erase();
  getStringTagValueRelaxed(RPMTAG_URL, pkg.url);
  //Packager may be omitted, no need to check return code; 
  pkg.packager.erase();
  getStringTagValueRelaxed(RPMTAG_PACKAGER, pkg.packager);
  //No i18n processing, is it required here?
  getStringTagValue(RPMTAG_SUMMARY, pkg.summary);
  //No i18n processing, is it required here?
  getStringTagValue(RPMTAG_DESCRIPTION, pkg.description);
}

/*
  int_32 count1 = 0, count2 = 0, count3 = 0, type = 0;
  char** names = NULL;
  char** versions = NULL;
  int_32* flags = NULL;
  int res = headerGetEntry(h, RPMTAG_PROVIDENAME, &type, (void **)&names, &count1);
  if (res == 0)//What exact constant must be used here?
    {
      headerFree(h);
      Fclose(fd);
      errMsg = "cannot get list of provide names";
      return 0;
    }
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(names);
  res = headerGetEntry(h, RPMTAG_PROVIDEVERSION, &type, (void **)&versions, &count2);
  if (res == 0)//What exact constant must be used here?
    {
      headerFree(h);
      Fclose(fd);
      errMsg = "cannot get list of provide versions";
      return 0;
    }
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(versions);
  res = headerGetEntry(h, RPMTAG_PROVIDEFLAGS, &type, (void **)&flags, &count3);
  if (res == 0)//What exact constant must be used here?
    {
      headerFree(h);
      Fclose(fd);
      errMsg = "cannot get list of provide flags";
      return 0;
    }
  assert(type == RPM_INT32_TYPE);
  assert(flags);
  assert(count1 == count2 && count2 == count3);
  p.provides.resize(count1);
  for(int_32 i = 0;i < count1;i++)
    p.provides[i] = PkgRel(names[i], versions[i], translateRelFlags(flags[i]));
*/


/*
  p.conflicts.clear();
  count1 = 0; count2 = 0; count3 = 0; type = 0;
  names = NULL; versions = NULL;
  flags = NULL;
  res = headerGetEntry(h, RPMTAG_CONFLICTNAME, &type, (void **)&names, &count1);
  if (res != 0)//What exact constant must be used here?
    {
      assert(type == RPM_STRING_ARRAY_TYPE);
      assert(names);
      res = headerGetEntry(h, RPMTAG_CONFLICTVERSION, &type, (void **)&versions, &count2);
      if (res == 0)//What exact constant must be used here?
	{
	  headerFree(h);
	  Fclose(fd);
	  errMsg = "cannot get list of conflict versions";
	  return 0;
	}
      assert(type == RPM_STRING_ARRAY_TYPE);
      assert(versions);
      res = headerGetEntry(h, RPMTAG_CONFLICTFLAGS, &type, (void **)&flags, &count3);
      if (res == 0)//What exact constant must be used here?
	{
	  headerFree(h);
	  Fclose(fd);
	  errMsg = "cannot get list of conflict flags";
	  return 0;
	}
      assert(type == RPM_INT32_TYPE);
      assert(flags);
      assert(count1 == count2 && count2 == count3);
      p.conflicts.resize(count1);
      for(int_32 i = 0;i < count1;i++)
	p.conflicts[i] = PkgRel(names[i], versions[i], translateRelFlags(flags[i]));
    }
*/

  /*
  p.obsoletes.clear();
  count1 = 0; count2 = 0; count3 = 0; type = 0;
  names = NULL; versions = NULL;
  flags = NULL;
  res = headerGetEntry(h, RPMTAG_OBSOLETENAME, &type, (void **)&names, &count1);
  if (res != 0)//What exact constant must be used here?
    {
      assert(type == RPM_STRING_ARRAY_TYPE);
      assert(names);
      res = headerGetEntry(h, RPMTAG_OBSOLETEVERSION, &type, (void **)&versions, &count2);
      if (res == 0)//What exact constant must be used here?
	{
	  headerFree(h);
	  Fclose(fd);
	  errMsg = "cannot get list of obsolete versions";
	  return 0;
	}
      assert(type == RPM_STRING_ARRAY_TYPE);
      assert(versions);
      res = headerGetEntry(h, RPMTAG_OBSOLETEFLAGS, &type, (void **)&flags, &count3);
      if (res == 0)//What exact constant must be used here?
	{
	  headerFree(h);
	  Fclose(fd);
	  errMsg = "cannot get list of obsolete flags";
	  return 0;
	}
      assert(type == RPM_INT32_TYPE);
      assert(flags);
      assert(count1 == count2 && count2 == count3);
      p.obsoletes.resize(count1);
      for(int_32 i = 0;i < count1;i++)
	p.obsoletes[i] = PkgRel(names[i], versions[i], translateRelFlags(flags[i]));
    }
  */

  /*
  count1 = 0; count2 = 0; count3 = 0; type = 0;
  names = NULL; versions = NULL;
  flags = NULL;
  res = headerGetEntry(h, RPMTAG_REQUIRENAME, &type, (void **)&names, &count1);
  if (res == 0)//What exact constant must be used here?
    {
      headerFree(h);
      Fclose(fd);
      errMsg = "cannot get list of require names";
      return 0;
    }
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(names);
  res = headerGetEntry(h, RPMTAG_REQUIREVERSION, &type, (void **)&versions, &count2);
  if (res == 0)//What exact constant must be used here?
    {
      headerFree(h);
      Fclose(fd);
      errMsg = "cannot get list of require versions";
      return 0;
    }
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(versions);
  res = headerGetEntry(h, RPMTAG_REQUIREFLAGS, &type, (void **)&flags, &count3);
  if (res == 0)//What exact constant must be used here?
    {
      headerFree(h);
      Fclose(fd);
      errMsg = "cannot get list of require flags";
      return 0;
    }
  assert(type == RPM_INT32_TYPE);
  assert(flags);
  assert(count1 == count2 && count2 == count3);
  for(int_32 i = 0;i < count1;i++)
    {
      if ((flags[i] & RPMSENSE_RPMLIB) == 0)//No need to handle rpmlib(feature) requirements
	p.requires.push_back (PkgRel(names[i], versions[i], translateRelFlags(flags[i])));
    }
  */

  /*
  StringVector dirNames;
  int_32* dirindexes = NULL;
  count1 = 0; count2 = 0; count3 = 0; type = 0;
  names = NULL; versions = NULL;
  flags = NULL;
  res = headerGetEntry(h, RPMTAG_DIRNAMES, &type, (void **)&names, &count1);
  if (res != 0)//What exact constant must be used here?
    {
      assert(type == RPM_STRING_ARRAY_TYPE);
      assert(names);
      dirNames.reserve(count1);
      for(int_32 i = 0;i < count1;i++)
	dirNames.push_back(names[i]);
      res = headerGetEntry(h, RPMTAG_DIRINDEXES, &type, (void **)&dirindexes, &count1);
      if (res == 0)//What exact constant must be used here?
	{
	  headerFree(h);
	  Fclose(fd);
	  errMsg = "cannot get list of directory indexes ";
	  return 0;
	}
      assert(type == RPM_INT32_TYPE);
      assert(dirindexes);
      names = NULL; versions = NULL;
      flags = NULL;
      res = headerGetEntry(h, RPMTAG_BASENAMES, &type, (void **)&names, &count2);
      if (res == 0)//What exact constant must be used here?
	{
	  headerFree(h);
	  Fclose(fd);
	  errMsg = "cannot get list of file basenames";
	  return 0;
	}
      assert(type == RPM_STRING_ARRAY_TYPE);
      assert(names);
      assert(count1 == count2);//count1 must have number of directory indexes
      for(int_32 i = 0;i < count2;i++)
	{
	  assert(dirindexes[i] < (int_32)dirNames.size());
	  const std::string value = concatUnixPath(dirNames[dirindexes[i]], names[i]);
	  if (value.find(" ") == std::string::npos)
	    p.provides.push_back(PkgRel(value));
	}
    }
  */

  /*
char translateRelFlags(int_32 flags)
{
  const bool less = flags & RPMSENSE_LESS, equal = flags & RPMSENSE_EQUAL, greater = flags & RPMSENSE_GREATER;
  assert(!less || !greater);
  if (less && equal)
    return PkgRel::LessOrEqual;
  if (greater && equal)
    return PkgRel::GreaterOrEqual;
  if (less)
    return PkgRel::Less;
  if (equal)
    return PkgRel::Equal;
  if (greater)
    return PkgRel::Greater;
  return 0;
}
*/

void RpmFileHeaderReader::getStringTagValue(int_32 tag, std::string& value)
{
  char* str;
  int_32 count, type;
  const int rc = headerGetEntry(m_header, tag, &type, (void**)&str, &count);
  if (rc == 0)//Is there proper constant ? RPMRC_OK is not suitable;
    {
      std::ostringstream ss;
      ss << "RPM header in \'" << m_fileName << "\' does not contain tag \'" << tag << "\'";
      RPM_STOP(ss.str());
    }
  if (count != 1)
    {
      std::ostringstream ss;
      ss << "RPM header in \'" << m_fileName << "\' has " << count << " values of tag \'" << tag << "\' but required 1";
      RPM_STOP(ss.str());
    }
  if (type != RPM_STRING_TYPE)
    {
      std::ostringstream ss;
      ss << "RPM header in \'" << m_fileName << "\' has tag \'" << tag << "\' of type \'" << type << "\' which is not a string";
      RPM_STOP(ss.str());
    }
  assert(str);
  value = str;
  return;
}

void RpmFileHeaderReader::getStringTagValueRelaxed(int_32 tag, std::string& value)
{
  char* str;
  int_32 count, type;
  const int rc = headerGetEntry(m_header, tag, &type, (void**)&str, &count);
  if (rc == 0)//Is there proper constant ? RPMRC_OK is not suitable;
    return;//Silently doing nothing;
  if (count != 1)
    {
      std::ostringstream ss;
      ss << "RPM header in \'" << m_fileName << "\' has " << count << " values of tag \'" << tag << "\' but required 1";
      RPM_STOP(ss.str());
    }
  if (type != RPM_STRING_TYPE)
    {
      std::ostringstream ss;
      ss << "RPM header in \'" << m_fileName << "\' has tag \'" << tag << "\' of type \'" << type << "\' which is not a string";
      RPM_STOP(ss.str());
    }
  assert(str);
  value = str;
  return;
}

void RpmFileHeaderReader::getInt32TagValueRelaxed(int_32 tag, int_32& value)
{
  int_32* num = NULL;
  int_32 count, type;
  const int rc = headerGetEntry(m_header, tag, &type, (void**)&num, &count);
  if (rc == 0)//Is there proper constant ? RPMRC_OK is not suitable;
    return;//Silently doing nothing;
  if (count != 1)
    {
      std::ostringstream ss;
      ss << "RPM header in \'" << m_fileName << "\' has " << count << " values of tag \'" << tag << "\' but required 1";
      RPM_STOP(ss.str());
    }
  if(type != RPM_INT32_TYPE)
    {
      std::ostringstream ss;
      ss << "RPM header in \'" << m_fileName << "\' has tag \'" << tag << "\' of type \'" << type << "\' which is not a string";
      RPM_STOP(ss.str());
    }
  assert(num);
  value = *num;
  return;
}
