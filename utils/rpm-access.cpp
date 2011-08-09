//Written with librpm-4.0.4-alt100.29;

#include<assert.h>
#include<list>
#include<vector>
#include<string>
#include<sstream>
#include<iostream>
#include<rpm/rpmlib.h>

class PkgRel
{
public:
  PkgRel(): flags(0) {}
  PkgRel(const std::string& n): name(n), flags(0) {}
  PkgRel(const std::string& n, const std::string& v): name(n), version(v), flags(0) {}
  PkgRel(const std::string& n, const std::string& v, int_32 f): name(n), version(v), flags(f) {}

  std::string name, version;
  int_32 flags;
}; //class PkgRel;

typedef std::list<PkgRel>  PkgRelList;
typedef std::vector<PkgRel> PkgRelVector;

//This class contains only data required for experiments and some general package information;
class Package
{
public:
  std::string name, epoch, version, release, arch, url, packager, summary, description;
  PkgRelVector requires, conflicts, provides;
}; //class Package;

std::ostream& operator <<(std::ostream& s, const Package& p)
{
  s << p.name << "-" << p.version << "-" << p.release;
  return s;
}

std::ostream& operator <<(std::ostream& s, const PkgRel& p)
{
  s << p.name << " " << p.version << " (" << p.flags << ")";
  return s;
}

bool getStringTagValue(Header h, int_32 tag, std::string& value, std::string& errMsg)
{
  char* str;
  int_32 count, type;
  const int rc = headerGetEntry(h, tag, &type, (void**)&str, &count);
  if (rc == 0)//Is there proper constant ? RPMRC_OK is not suitable;
    {
      errMsg = "cannot get rpm package tag value";
      return 0;
    }
  if (count != 1)
    {
      std::ostringstream ss;
      ss << "received " << count << " lines of rpm package tag value, but must be one";
      errMsg = ss.str();
      return 0;
    }
  assert(type == RPM_STRING_TYPE);
  assert(str);
  value = str;
  return 1;
}

bool readPackageData(const std::string fileName, Package& p, std::string& errMsg)
{
  FD_t fd = Fopen(fileName.c_str(), "r");
  if (fd == NULL)
    {
      errMsg = "cannot open \'" + fileName + "\' for reading";
      return 0;
    }
  Header h;
  rpmRC rc = rpmReadPackageHeader(fd, &h, 0, NULL, NULL);
  if (rc != RPMRC_OK || h == NULL)
    {
      if (h)
	headerFree(h);
      Fclose(fd);
      errMsg = "cannot read package header from \'" + fileName + "\'";
      return 0;

    }
  //FIXME:rc checking;

  if (!getStringTagValue(h, RPMTAG_NAME, p.name, errMsg))
    {
      headerFree(h);
      Fclose(fd);
      return 0;
    }

  //Epoch may be omitted, no need to check return code; 
  p.epoch.erase();
  getStringTagValue(h, RPMTAG_EPOCH, p.epoch, errMsg);

  if (!getStringTagValue(h, RPMTAG_VERSION, p.version, errMsg))
    {
      headerFree(h);
      Fclose(fd);
      return 0;
    }

  if (!getStringTagValue(h, RPMTAG_RELEASE, p.release, errMsg))
    {
      headerFree(h);
      Fclose(fd);
      return 0;
    }

  if (!getStringTagValue(h, RPMTAG_ARCH, p.arch, errMsg))
    {
      headerFree(h);
      Fclose(fd);
      return 0;
    }

  if (!getStringTagValue(h, RPMTAG_URL, p.url, errMsg))
    {
      headerFree(h);
      Fclose(fd);
      return 0;
    }

  if (!getStringTagValue(h, RPMTAG_PACKAGER, p.packager, errMsg))
    {
      headerFree(h);
      Fclose(fd);
      return 0;
    }

  //No i18n processing, is it required here?
  if (!getStringTagValue(h, RPMTAG_SUMMARY, p.summary, errMsg))
    {
      headerFree(h);
      Fclose(fd);
      return 0;
    }

  //No i18n processing, is it required here?
  if (!getStringTagValue(h, RPMTAG_DESCRIPTION, p.description, errMsg))
    {
      headerFree(h);
      Fclose(fd);
      return 0;
    }

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
    p.provides[i] = PkgRel(names[i], versions[i], flags[i]);

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
  p.requires.resize(count1);
  for(int_32 i = 0;i < count1;i++)
    p.requires[i] = PkgRel(names[i], versions[i], flags[i]);
  /*FIXME:PreReq:Assuming PreReq is the same as usual requires;
  names = NULL;
  count1 = 0;
  type = 0;
  res = headerGetEntry(h, RPMTAG_PREREQ, &type, (void **)&names, &count1);
  if (res == 0)//What exact constant must be used here?
    {
      headerFree(h);
      Fclose(fd);
      errMsg = "cannot get list of prereq names";
      return 0;
    }
  assert(type == RPM_STRING_ARRAY_TYPE);
  assert(names);
  for(int_32 i = 0;i < count1;i++)
    p.requires.push_back(PkgRel(names[i]));
  */

  headerFree(h);
  Fclose(fd);
  return 1;
}

int main(int argc, char* argv[])
{
  assert(argc >= 2);
  Package p;
  std::string errMsg;
  if (!readPackageData(argv[1], p, errMsg))
    {
      std::cerr << "error:" << errMsg << std::endl;
	return 1;
    }
  std::cout << p << std::endl;
  std::cout << "Provides:" << std::endl;
  for(PkgRelVector::size_type i = 0;i < p.provides.size();i++)
    std::cout << p.provides[i] << std::endl;

  std::cout << "Requires:" << std::endl;
  for(PkgRelVector::size_type i = 0;i < p.requires.size();i++)
    std::cout << p.requires[i] << std::endl;

  return 0;
}
