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

#include"deepsolver.h"
#include"RpmException.h"
#include"RpmHeaderReading.h"

static VerDirection translateRelFlags(int_32 flags)
{
  VerDirection value = 0;
  if (flags & RPMSENSE_LESS)
    value |= VerLess;
  if (flags & RPMSENSE_EQUAL)
    value |= VerEquals;
  if (flags & RPMSENSE_GREATER)
    value |= VerGreater;
  return value;
}

void rpmFillMainData(Header& header, PkgFileBase& pkg)
{
  rpmGetStringTagValue(header, RPMTAG_NAME, pkg.name);
  int32_t epoch = 0;
  rpmGetInt32TagValueRelaxed(header, RPMTAG_EPOCH, epoch);
  pkg.epoch = epoch;
  rpmGetStringTagValue(header, RPMTAG_VERSION, pkg.version);
  rpmGetStringTagValue(header, RPMTAG_RELEASE, pkg.release);
  rpmGetStringTagValue(header, RPMTAG_ARCH, pkg.arch);
  pkg.url.erase();
  rpmGetStringTagValueRelaxed(header, RPMTAG_URL, pkg.url);
  pkg.packager.erase();
  rpmGetStringTagValueRelaxed(header, RPMTAG_PACKAGER, pkg.packager);
  pkg.license.erase();
  rpmGetStringTagValueRelaxed(header, RPMTAG_LICENSE, pkg.license);
  pkg.srcRpm.erase();
  rpmGetStringTagValueRelaxed(header, RPMTAG_SOURCERPM, pkg.srcRpm);
  //No i18n processing, is it required here?
  rpmGetStringTagValue(header, RPMTAG_SUMMARY, pkg.summary);
  //No i18n processing, is it required here?
  rpmGetStringTagValue(header, RPMTAG_DESCRIPTION, pkg.description);
  int32_t buildTime = 0;
  rpmGetInt32TagValueRelaxed(header, RPMTAG_BUILDTIME, buildTime);
  pkg.buildTime = buildTime;
}

void rpmFillProvides(Header& header, NamedPkgRelVector& v)
{
  v.clear();
  int_32 count1 = 0, count2 = 0, count3 = 0, type = 0;
  char** names = NULL;
  char** versions = NULL;
  int_32* flags = NULL;
  int res = headerGetEntry(header, RPMTAG_PROVIDENAME, &type, (void **)&names, &count1);
  if (res == 0)//What exact constant must be used here?
    return;//No provides entries found;
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(names);
  res = headerGetEntry(header, RPMTAG_PROVIDEVERSION, &type, (void **)&versions, &count2);
  if (res == 0)//What exact constant must be used here?
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      //FIXME:      RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains list of provides but does not contain list of provide versions");
      assert(0);
    }
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(versions);
  res = headerGetEntry(header, RPMTAG_PROVIDEFLAGS, &type, (void **)&flags, &count3);
  if (res == 0)//What exact constant must be used here?
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
      //FIXME:      RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains list of provides but does not contain list of provide flags");
      assert(0);
    }
  assert(type == RPM_INT32_TYPE);
  assert(flags);
  if (count1 != count2 || count1 != count3)
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
      //FIXME:      RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains different number of items in provide name list, provide version list and provide flags list");
      assert(0);
    }
  for(int_32 i = 0;i < count1;i++)
    v.push_back(NamedPkgRel(names[i], translateRelFlags(flags[i]), versions[i]));
  headerFreeData(names, RPM_STRING_ARRAY_TYPE);
  headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
}

void rpmFillConflicts(Header& header, NamedPkgRelVector& v)
{
  v.clear();
  int_32 count1 = 0, count2 = 0, count3 = 0, type = 0;
  char** names = NULL;
  char** versions = NULL;
  int_32* flags = NULL;
  int res = headerGetEntry(header, RPMTAG_CONFLICTNAME, &type, (void **)&names, &count1);
  if (res == 0)//What exact constant must be used here?
    return;
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(names);
  res = headerGetEntry(header, RPMTAG_CONFLICTVERSION, &type, (void **)&versions, &count2);
  if (res == 0)//What exact constant must be used here?
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      //FIXME:      RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains list of conflict names but does not contain list of conflict versions");
      assert(0);
    }
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(versions);
  res = headerGetEntry(header, RPMTAG_CONFLICTFLAGS, &type, (void **)&flags, &count3);
  if (res == 0)//What exact constant must be used here?
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
      //FIXME:      RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains list of conflict names but does not contain list of conflict flags");
      assert(0);
    }
  assert(type == RPM_INT32_TYPE);
  assert(flags);
  if (count1 != count2 || count1 != count3)
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
      //FIXME:      RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains different number of items in conflict name list, conflict version list and conflict flags list");
      assert(0);
    }
  for(int_32 i = 0;i < count1;i++)
    v.push_back(NamedPkgRel(names[i], translateRelFlags(flags[i]), versions[i]));
  headerFreeData(names, RPM_STRING_ARRAY_TYPE);
  headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
}

void rpmFillObsoletes(Header& header, NamedPkgRelVector& v)
{
  v.clear();
  int_32 count1 = 0, count2 = 0, count3 = 0, type = 0;
  char** names = NULL;
  char** versions = NULL;
  int_32* flags = NULL;
  int res = headerGetEntry(header, RPMTAG_OBSOLETENAME, &type, (void **)&names, &count1);
  if (res == 0)//What exact constant must be used here?
    return;
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(names);
  res = headerGetEntry(header, RPMTAG_OBSOLETEVERSION, &type, (void **)&versions, &count2);
  if (res == 0)//What exact constant must be used here?
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      //FIXME:      RPM_STOP("Header of RPM file \'" + m_fileName + " contains obsolete name list but does not contain obsolete version list");
      assert(0);
    }
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(versions);
  res = headerGetEntry(header, RPMTAG_OBSOLETEFLAGS, &type, (void **)&flags, &count3);
  if (res == 0)//What exact constant must be used here?
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
      //FIXME:      RPM_STOP("Header of RPM file \'" + m_fileName + " contains obsolete name list but does not contain obsolete flags list");
      assert(0);
    }
  assert(type == RPM_INT32_TYPE);
  assert(flags);
  if (count1 != count2 || count1 != count3)
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
      //FIXME:      RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains different number of items in obsolete name list, obsolete version list and obsolete flags list");
      assert(0);
    }
  for(int_32 i = 0;i < count1;i++)
    v.push_back(NamedPkgRel(names[i], translateRelFlags(flags[i]), versions[i]));
  headerFreeData(names, RPM_STRING_ARRAY_TYPE);
  headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
}

void rpmFillRequires(Header& header, NamedPkgRelVector& v)
{
  v.clear();
  int_32 count1 = 0, count2 = 0, count3 = 0, type = 0;
  char** names = NULL;
  char** versions = NULL;
  int_32* flags = NULL;
  int res = headerGetEntry(header, RPMTAG_REQUIRENAME, &type, (void **)&names, &count1);
  if (res == 0)//What exact constant must be used here?
    return;
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(names);
  res = headerGetEntry(header, RPMTAG_REQUIREVERSION, &type, (void **)&versions, &count2);
  if (res == 0)//What exact constant must be used here?
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      //FIXME:      RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains require name list but does not contain require version list");
      assert(0);
    }
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(versions);
  res = headerGetEntry(header, RPMTAG_REQUIREFLAGS, &type, (void **)&flags, &count3);
  if (res == 0)//What exact constant must be used here?
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
      //FIXME:      RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains require name list but does not contain require flags list");
      assert(0);
    }
  assert(type == RPM_INT32_TYPE);
  assert(flags);
  if (count1 != count2 || count1 != count3)
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
      //FIXME:    RPM_STOP("Header of rpm file \'" + m_fileName + "\' contains different number of items in require name list, require version list and require flags list");
      assert(0);
    }
  for(int_32 i = 0;i < count1;i++)
    v.push_back (NamedPkgRel(names[i], translateRelFlags(flags[i]), versions[i]));
  headerFreeData(names, RPM_STRING_ARRAY_TYPE);
  headerFreeData(versions, RPM_STRING_ARRAY_TYPE);
}

void rpmFillChangeLog(Header& header, ChangeLog& changeLog)
{
  changeLog.clear();
  int_32 count = 0;
  int_32 type = 0;
  char** text = NULL;
  int res;
  res = headerGetEntry(header, RPMTAG_CHANGELOGTEXT, &type, (void **)&text, &count);
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
  res = headerGetEntry(header, RPMTAG_CHANGELOGNAME, &type, (void **)&text, &count);
  if (res == 0)//What exact constant must be used here?
    //FIXME:    RPM_STOP("RPM file \'" + m_fileName + "\' does not contain change log names but contains change log text");
    assert(0);
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
  res = headerGetEntry(header, RPMTAG_CHANGELOGTIME, &type, (void **)&time, &count);
  if (res == 0)//What exact constant must be used here?
    //FIXME:    RPM_STOP("RPM file \'" + m_fileName + "\' does not contain change log time entries but contains change log text entries");
    assert(0);
  assert(type == RPM_INT32_TYPE);
  assert(time);
  assert((size_t)count == changeLog.size());
  for(ChangeLog::size_type i = 0;i < changeLog.size();i++)
    changeLog[i].time = time[i];
}

void rpmFillFileList(Header& header, StringList& v)
{
  v.clear();
  StringVector dirNames;
  int_32* dirIndexes = NULL;
  int_32 count1 = 0, count2 = 0, type = 0;
  char** names = NULL;
  int res = headerGetEntry(header, RPMTAG_DIRNAMES, &type, (void **)&names, &count1);
  if (res == 0)//What exact constant must be used here?
    return;
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(names);
  dirNames.reserve(count1);
  for(int_32 i = 0;i < count1;i++)
    dirNames.push_back(names[i]);
  headerFreeData(names, RPM_STRING_ARRAY_TYPE);
  res = headerGetEntry(header, RPMTAG_DIRINDEXES, &type, (void **)&dirIndexes, &count1);
  if (res == 0)//What exact constant must be used here?
    //FIXME:    RPM_STOP("Header of rpm file \'" + m_fileName + "\' does not contain directory indices tag but has list of directory names");
    assert(0);
  assert(type == RPM_INT32_TYPE);
  assert(dirIndexes);
  res = headerGetEntry(header, RPMTAG_BASENAMES, &type, (void **)&names, &count2);
  if (res == 0)//What exact constant must be used here?
    //FIXME:    RPM_STOP("Header of rpm file \'" + m_fileName + "\' does not contain list of stored file base names  but has list of directories");
    assert(0);
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(names);
  if (count1 != count2)
    {
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
      //FIXME:      RPM_STOP("Header of rpm file \'" + m_fileName + "\' has different number of stored files and directory indices for them");
      assert(0);
    }
  for(int_32 i = 0;i < count2;i++)
    {
      assert(dirIndexes[i] < (int_32)dirNames.size());
      v.push_back(Directory::mixNameComponents(dirNames[dirIndexes[i]], names[i]));
    }
      headerFreeData(names, RPM_STRING_ARRAY_TYPE);
}

void rpmGetStringTagValue(Header& header, int_32 tag, std::string& value)
{
  char* str;
  int_32 count, type;
  const int rc = headerGetEntry(header, tag, &type, (void**)&str, &count);
  if (rc == 0)//Is there proper constant ? RPMRC_OK is not suitable;
    {
      /*FIXME:      std::ostringstream ss;
      ss << "RPM header in \'" << m_fileName << "\' does not contain tag \'" << tag << "\'";
      RPM_STOP(ss.str());
      */
      assert(0);
    }
  if (count != 1)
    {
      /*FIXME:      std::ostringstream ss;
      ss << "RPM header in \'" << m_fileName << "\' has " << count << " values of tag \'" << tag << "\' but required 1";
      RPM_STOP(ss.str());
      */
      assert(0);
    }
  if (type != RPM_STRING_TYPE)
    {
      /*FIXME:
      std::ostringstream ss;
      ss << "RPM header in \'" << m_fileName << "\' has tag \'" << tag << "\' of type \'" << type << "\' which is not a string";
      RPM_STOP(ss.str());
      */
      assert(0);
    }
  assert(str);
  value = str;
  return;
}

void rpmGetStringTagValueRelaxed(Header& header, int_32 tag, std::string& value)
{
  char* str;
  int_32 count, type;
  const int rc = headerGetEntry(header, tag, &type, (void**)&str, &count);
  if (rc == 0)//Is there proper constant ? RPMRC_OK is not suitable;
    return;//Silently doing nothing;
  if (count != 1)
    {
      /*FIXME:
      std::ostringstream ss;
      ss << "RPM header in \'" << m_fileName << "\' has " << count << " values of tag \'" << tag << "\' but required 1";
      RPM_STOP(ss.str());
      */
      assert(0);
    }
  if (type != RPM_STRING_TYPE)
    {
      /*FIXME:
      std::ostringstream ss;
      ss << "RPM header in \'" << m_fileName << "\' has tag \'" << tag << "\' of type \'" << type << "\' which is not a string";
      RPM_STOP(ss.str());
      */
      assert(0);
    }
  assert(str);
  value = str;
  return;
}

void rpmGetInt32TagValueRelaxed(Header& header , int_32 tag, int_32& value)
{
  int_32* num = NULL;
  int_32 count, type;
  const int rc = headerGetEntry(header, tag, &type, (void**)&num, &count);
  if (rc == 0)//Is there proper constant ? RPMRC_OK is not suitable;
    return;//Silently doing nothing;
  if (count != 1)
    {
      /*FIXME:
      std::ostringstream ss;
      ss << "RPM header in \'" << m_fileName << "\' has " << count << " values of tag \'" << tag << "\' but required 1";
      RPM_STOP(ss.str());
      */
      assert(0);
    }
  if(type != RPM_INT32_TYPE)
    {
      /*FIXME:
      std::ostringstream ss;
      ss << "RPM header in \'" << m_fileName << "\' has tag \'" << tag << "\' of type \'" << type << "\' which is not a string";
      RPM_STOP(ss.str());
      */
      assert(0);
    }
  assert(num);
  value = *num;
  return;
}
