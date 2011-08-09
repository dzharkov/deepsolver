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
  //FIXME:type checking;
  assert(str);
  value = str;
  return 1;
}

bool readPackageData(const std::string fileName, Package& p, std::string& errMsg)
{
  FD_t fd = Fopen(fileName.c_str(), "r");
  assert(fd != NULL);
  //FIXME:rc checking;
  Header h;
  rpmRC rc = rpmReadPackageHeader(fd, &h, 0, NULL, NULL);
  assert(rc == 0);
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

  /*
  char **namel = NULL;

  rc = headerGetEntry(h, RPMTAG_PROVIDENAME, &type, (void **)&namel, &count);
  std::cout << count << std::endl;
  for(size_t i = 0;i < count;i++)
    std::cout << namel[i] << std::endl;

  rc = headerGetEntry(h, RPMTAG_REQUIRENAME, &type, (void **)&namel, &count);
  std::cout << count << std::endl;
  for(size_t i = 0;i < count;i++)
    std::cout << namel[i] << std::endl;

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
  return 0;
}
