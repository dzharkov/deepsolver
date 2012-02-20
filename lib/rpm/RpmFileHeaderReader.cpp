//Written with librpm-4.0.4-alt100.29;
//FIXME:requires just for install/removing
//FIXME:package size and required disk space;

#include"deepsolver.h"
#include"RpmFileHeaderReader.h"

//FIXME:requires flags for package install and removing;

static char translateRelFlags(int_32 flags)
{
  char value = 0;
  if (flags & RPMSENSE_LESS)
    value |= VerLess;
  if (flags & RPMSENSE_EQUAL)
    value |= VerEquals;
  if (flags & RPMSENSE_GREATER)
    value |= VerGreater;
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

void RpmFileHeaderReader::fillMainData(PkgFileBase& pkg)
{
  getStringTagValue(RPMTAG_NAME, pkg.name);
  int32_t epoch = 0;
  getInt32TagValueRelaxed(RPMTAG_EPOCH, epoch);
  pkg.epoch = epoch;
  getStringTagValue(RPMTAG_VERSION, pkg.version);
  getStringTagValue(RPMTAG_RELEASE, pkg.release);
  getStringTagValue(RPMTAG_ARCH, pkg.arch);
  pkg.url.erase();
  getStringTagValueRelaxed(RPMTAG_URL, pkg.url);
  pkg.packager.erase();
  getStringTagValueRelaxed(RPMTAG_PACKAGER, pkg.packager);
  pkg.license.erase();
  getStringTagValueRelaxed(RPMTAG_LICENSE, pkg.license);
  pkg.srcRpm.erase();
  getStringTagValueRelaxed(RPMTAG_SOURCERPM, pkg.srcRpm);
  //No i18n processing, is it required here?
  getStringTagValue(RPMTAG_SUMMARY, pkg.summary);
  //No i18n processing, is it required here?
  getStringTagValue(RPMTAG_DESCRIPTION, pkg.description);
  int32_t buildTime = 0;
  getInt32TagValueRelaxed(RPMTAG_BUILDTIME, buildTime);
  pkg.buildTime = buildTime;
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
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains list of provides but does not contain list of provide versions");
    }
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(versions);
  res = headerGetEntry(m_header, RPMTAG_PROVIDEFLAGS, &type, (void **)&flags, &count3);
  if (res == 0)//What exact constant must be used here?
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
      RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains list of provides but does not contain list of provide flags");
    }
  assert(type == RPM_INT32_TYPE);
  assert(flags);
  if (count1 != count2 || count1 != count3)
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
      RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains different number of items in provide name list, provide version list and provide flags list");
    }
  for(int_32 i = 0;i < count1;i++)
    v.push_back(NamedPkgRel(names[i], translateRelFlags(flags[i]), versions[i]));
  headerFreeData(names, RPM_STRING_ARRAY_TYPE);
  headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
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
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains list of conflict names but does not contain list of conflict versions");
    }
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(versions);
  res = headerGetEntry(m_header, RPMTAG_CONFLICTFLAGS, &type, (void **)&flags, &count3);
  if (res == 0)//What exact constant must be used here?
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
      RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains list of conflict names but does not contain list of conflict flags");
    }
  assert(type == RPM_INT32_TYPE);
  assert(flags);
  if (count1 != count2 || count1 != count3)
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
      RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains different number of items in conflict name list, conflict version list and conflict flags list");
    }
  for(int_32 i = 0;i < count1;i++)
    v.push_back(NamedPkgRel(names[i], translateRelFlags(flags[i]), versions[i]));
  headerFreeData(names, RPM_STRING_ARRAY_TYPE);
  headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
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
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      RPM_STOP("Header of RPM file \'" + m_fileName + " contains obsolete name list but does not contain obsolete version list");
    }
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(versions);
  res = headerGetEntry(m_header, RPMTAG_OBSOLETEFLAGS, &type, (void **)&flags, &count3);
  if (res == 0)//What exact constant must be used here?
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
      RPM_STOP("Header of RPM file \'" + m_fileName + " contains obsolete name list but does not contain obsolete flags list");
    }
  assert(type == RPM_INT32_TYPE);
  assert(flags);
  if (count1 != count2 || count1 != count3)
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
      RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains different number of items in obsolete name list, obsolete version list and obsolete flags list");
    }
  for(int_32 i = 0;i < count1;i++)
    v.push_back(NamedPkgRel(names[i], translateRelFlags(flags[i]), versions[i]));
  headerFreeData(names, RPM_STRING_ARRAY_TYPE);
  headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
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
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains require name list but does not contain require version list");
    }
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(versions);
  res = headerGetEntry(m_header, RPMTAG_REQUIREFLAGS, &type, (void **)&flags, &count3);
  if (res == 0)//What exact constant must be used here?
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
      RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains require name list but does not contain require flags list");
    }
  assert(type == RPM_INT32_TYPE);
  assert(flags);
  if (count1 != count2 || count1 != count3)
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
    RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains different number of items in require name list, require version list and require flags list");
    }
  for(int_32 i = 0;i < count1;i++)
    v.push_back (NamedPkgRel(names[i], translateRelFlags(flags[i]), versions[i]));
  headerFreeData(names, RPM_STRING_ARRAY_TYPE);
  headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
}

void RpmFileHeaderReader::fillChangeLog(ChangeLog& changeLog)
{
  changeLog.clear();
  int_32 count = 0;
  int_32 type = 0;
  char** text = NULL;
  int res;
  res = headerGetEntry(m_header, RPMTAG_CHANGELOGTEXT, &type, (void **)&text, &count);
  if (res == 0)//What exact constant must be used here?
    return;
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(text);
  changeLog.resize(count);
  for(ChangeLog::size_type i = 0;i < changeLog.size();i++)
    {
      assert(text[i]);
      changeLog[i].text = text[i];
    }
  headerFreeData(text, RPM_STRING_ARRAY_TYPE);
  res = headerGetEntry(m_header, RPMTAG_CHANGELOGNAME, &type, (void **)&text, &count);
  if (res == 0)//What exact constant must be used here?
    RPM_STOP("RPM file \'" + m_fileName + "\' does not contain change log names but contains change log text");
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(text);
  assert((size_t)count == changeLog.size());
  for(ChangeLog::size_type i = 0;i < changeLog.size();i++)
    {
      assert(text[i]);
      changeLog[i].name = text[i];
    }
  headerFreeData(text, RPM_STRING_ARRAY_TYPE);
  int_32* time = NULL;
  res = headerGetEntry(m_header, RPMTAG_CHANGELOGTIME, &type, (void **)&time, &count);
  if (res == 0)//What exact constant must be used here?
    RPM_STOP("RPM file \'" + m_fileName + "\' does not contain change log time entries but contains change log text entries");
  assert(type == RPM_INT32_TYPE);
  assert(time);
  assert((size_t)count == changeLog.size());
  for(ChangeLog::size_type i = 0;i < changeLog.size();i++)
    changeLog[i].time = time[i];
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
  headerFreeData(names, RPM_STRING_ARRAY_TYPE);
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
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      RPM_STOP("Header of rpm file \'" + m_fileName + "\' has different number of stored files and directory indices for them");
    }
  for(int_32 i = 0;i < count2;i++)
    {
      assert(dirIndexes[i] < (int_32)dirNames.size());
	  v.push_back(concatUnixPath(dirNames[dirIndexes[i]], names[i]));
    }
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
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

