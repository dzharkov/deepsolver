//Written with librpm-4.0.4-alt100.29;

#include"../basic-header.h"//FIXME:
#include"RpmFileHeaderReader.h"

static char translateRelFlags(int_32 flags)
{
  char value = 0;
  if (flags & RPMSENSE_LESS)
    value |= NamedPkgRel::Less;
  if (flags & RPMSENSE_EQUAL)
    value |= NamedPkgRel::Equals;
  if (flags & RPMSENSE_GREATER)
    value |= NamedPkgRel::Greater;
  return value;
}

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

void RpmFileHeaderReader::fillProvides(NamedPkgRelVector& v)
{
  v.clear();
  int_32 count1 = 0, count2 = 0, count3 = 0, type = 0;
  char** names = NULL;
  char** versions = NULL;
  int_32* flags = NULL;
  int res = headerGetEntry(m_header, RPMTAG_PROVIDENAME, &type, (void **)&names, &count1);
  if (res == 0)//What exact constant must be used here?
    return;//No provides entries found;
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(names);
  res = headerGetEntry(m_header, RPMTAG_PROVIDEVERSION, &type, (void **)&versions, &count2);
  if (res == 0)//What exact constant must be used here?
    RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains list of provides but does not contain list of provide versions");
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(versions);
  res = headerGetEntry(m_header, RPMTAG_PROVIDEFLAGS, &type, (void **)&flags, &count3);
  if (res == 0)//What exact constant must be used here?
    RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains list of provides but does not contain list of provide flags");
  assert(type == RPM_INT32_TYPE);
  assert(flags);
  if (count1 != count2 || count1 != count3)
    RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains different number of items in provide name list, provide version list and provide flags list");
  v.resize(count1);
  for(int_32 i = 0;i < count1;i++)
    v[i] = NamedPkgRel(names[i], translateRelFlags(flags[i]), versions[i]);
}

void RpmFileHeaderReader::fillConflicts(NamedPkgRelVector& v)
{
  v.clear();
  int_32 count1 = 0, count2 = 0, count3 = 0, type = 0;
  char** names = NULL;
  char** versions = NULL;
  int_32* flags = NULL;
  int res = headerGetEntry(m_header, RPMTAG_CONFLICTNAME, &type, (void **)&names, &count1);
  if (res == 0)//What exact constant must be used here?
    return;
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(names);
  res = headerGetEntry(m_header, RPMTAG_CONFLICTVERSION, &type, (void **)&versions, &count2);
  if (res == 0)//What exact constant must be used here?
    RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains list of conflict names but does not contain list of conflict versions");
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(versions);
  res = headerGetEntry(m_header, RPMTAG_CONFLICTFLAGS, &type, (void **)&flags, &count3);
  if (res == 0)//What exact constant must be used here?
    RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains list of conflict names but does not contain list of conflict flags");
  assert(type == RPM_INT32_TYPE);
  assert(flags);
  if (count1 != count2 || count1 != count3)
    RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains different number of items in conflict name list, conflict version list and conflict flags list");
  v.resize(count1);
  for(int_32 i = 0;i < count1;i++)
    v[i] = NamedPkgRel(names[i], translateRelFlags(flags[i]), versions[i]);
}

void RpmFileHeaderReader::fillObsoletes(NamedPkgRelVector& v)
{
  v.clear();
  int_32 count1 = 0, count2 = 0, count3 = 0, type = 0;
  char** names = NULL;
  char** versions = NULL;
  int_32* flags = NULL;
  int res = headerGetEntry(m_header, RPMTAG_OBSOLETENAME, &type, (void **)&names, &count1);
  if (res == 0)//What exact constant must be used here?
    return;
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(names);
  res = headerGetEntry(m_header, RPMTAG_OBSOLETEVERSION, &type, (void **)&versions, &count2);
  if (res == 0)//What exact constant must be used here?
    RPM_STOP("Header of RPM file \'" + m_fileName + " contains obsolete name list but does not contain obsolete version list");
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(versions);
  res = headerGetEntry(m_header, RPMTAG_OBSOLETEFLAGS, &type, (void **)&flags, &count3);
  if (res == 0)//What exact constant must be used here?
    RPM_STOP("Header of RPM file \'" + m_fileName + " contains obsolete name list but does not contain obsolete flags list");
  assert(type == RPM_INT32_TYPE);
  assert(flags);
  if (count1 != count2 || count1 != count3)
    RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains different number of items in obsolete name list, obsolete version list and obsolete flags list");
  v.resize(count1);
  for(int_32 i = 0;i < count1;i++)
    v[i] = NamedPkgRel(names[i], translateRelFlags(flags[i]), versions[i]);
}

void RpmFileHeaderReader::fillRequires(NamedPkgRelVector& v)
{
  v.clear();
  int_32 count1 = 0, count2 = 0, count3 = 0, type = 0;
  char** names = NULL;
  char** versions = NULL;
  int_32* flags = NULL;
  int res = headerGetEntry(m_header, RPMTAG_REQUIRENAME, &type, (void **)&names, &count1);
  if (res == 0)//What exact constant must be used here?
    return;
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(names);
  res = headerGetEntry(m_header, RPMTAG_REQUIREVERSION, &type, (void **)&versions, &count2);
  if (res == 0)//What exact constant must be used here?
    RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains require name list but does not contain require version list");
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(versions);
  res = headerGetEntry(m_header, RPMTAG_REQUIREFLAGS, &type, (void **)&flags, &count3);
  if (res == 0)//What exact constant must be used here?
    RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains require name list but does not contain require flags list");
  assert(type == RPM_INT32_TYPE);
  assert(flags);
  if (count1 != count2 || count1 != count3)
    RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains different number of items in require name list, require version list and require flags list");
  for(int_32 i = 0;i < count1;i++)
    {
      if ((flags[i] & RPMSENSE_RPMLIB) == 0)//No need to handle rpmlib(feature) requirements
	v.push_back (NamedPkgRel(names[i], translateRelFlags(flags[i]), versions[i]));
    }
}

void RpmFileHeaderReader::fillFileList(StringList& v)
{
  v.clear();
  StringVector dirNames;
  int_32* dirIndexes = NULL;
  int_32 count1 = 0, count2 = 0, type = 0;
  char** names = NULL;
  int res = headerGetEntry(m_header, RPMTAG_DIRNAMES, &type, (void **)&names, &count1);
  if (res == 0)//What exact constant must be used here?
    return;
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(names);
  dirNames.reserve(count1);
  for(int_32 i = 0;i < count1;i++)
    dirNames.push_back(names[i]);
  res = headerGetEntry(m_header, RPMTAG_DIRINDEXES, &type, (void **)&dirIndexes, &count1);
  if (res == 0)//What exact constant must be used here?
    RPM_STOP("Header of rpm file \'" + m_fileName + "\' does not contain directory indices tag but has list of directory names");
  assert(type == RPM_INT32_TYPE);
  assert(dirIndexes);
  res = headerGetEntry(m_header, RPMTAG_BASENAMES, &type, (void **)&names, &count2);
  if (res == 0)//What exact constant must be used here?
    RPM_STOP("Header of rpm file \'" + m_fileName + "\' does not contain list of stored file base names  but has list of directories");
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(names);
  if (count1 != count2)
    RPM_STOP("Header of rpm file \'" + m_fileName + "\' has different number of stored files and directory indices for them");
  for(int_32 i = 0;i < count2;i++)
    {
      assert(dirIndexes[i] < (int_32)dirNames.size());
	  v.push_back(concatUnixPath(dirNames[dirIndexes[i]], names[i]));
    }
}

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

bool readRpmPkgFile(const std::string& fileName
		 PkgFile& pkgFile,
		 NamedPkgRelList& provides,
		 NamedPkgRelList& requires,
		 NamedPkgRelList& conflicts,
		 NamedPkgRelList& obsoletes)
{
  //FIXME:
  return 0;
}

