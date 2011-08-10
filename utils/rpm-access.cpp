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
  enum {
    Less = 1,
    LessOrEqual = 2,
    Equal = 3,
    GreaterOrEqual = 4,
    Greater = 5
  };

  PkgRel(): flags(0), versionRel(0) {}
  PkgRel(const std::string& n): name(n), flags(0), versionRel(0) {}
  PkgRel(const std::string& n, const std::string& v): name(n), version(v), flags(0), versionRel(0) {}
  PkgRel(const std::string& n, const std::string& v, int_32 f): name(n), version(v), flags(f), versionRel(0) {}

  std::string name, version;
  int_32 flags;
  char versionRel;
}; //class PkgRel;

typedef std::list<PkgRel>  PkgRelList;
typedef std::vector<PkgRel> PkgRelVector;

//This class contains only data required for experiments and some general package information;
class Package
{
public:
  std::string name, epoch, version, release, arch, url, packager, summary, description;
  PkgRelVector requires, conflicts, provides, obsoletes;
}; //class Package;

std::ostream& operator <<(std::ostream& s, const Package& p)
{
  s << p.name << "-" << p.version << "-" << p.release;
  return s;
}

std::ostream& operator <<(std::ostream& s, const PkgRel& p)
{
  s << p.name;
  if (p.versionRel)
    {
      assert(!p.version.empty());
      s << " ";
      switch(p.versionRel)
	{
	case PkgRel::Less:
	  s << "<";
	  break;
	case PkgRel::LessOrEqual:
	  s << "<=";
	  break;
	case PkgRel::Equal:
	  s << "=";
	  break;
	case PkgRel::GreaterOrEqual:
	  s << ">=";
	  break;
	case PkgRel::Greater:
	  s << ">";
	  break;
	default:
	  assert(0);
	} //switch(p.versionRel); 
      s << " " << p.version;
    } else 
    {
      assert(p.version.empty());
    }
  s << " (" << p.flags << ")";
  return s;
}

void translateRelFlags(PkgRel& p)
{
  const bool less = p.flags & RPMSENSE_LESS, equal = p.flags & RPMSENSE_EQUAL, greater = p.flags & RPMSENSE_GREATER;
  assert(!less || !greater);
  if (less && equal)
    p.versionRel = PkgRel::LessOrEqual; else
  if (greater && equal)
    p.versionRel = PkgRel::GreaterOrEqual; else
  if (less)
    p.versionRel = PkgRel::Less; else
  if (equal)
    p.versionRel = PkgRel::Equal; else
  if (greater)
    p.versionRel = PkgRel::Greater; else
    p.versionRel = 0;
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

  //URL may be omitted, no need to check return code; 
  p.url.erase();
  getStringTagValue(h, RPMTAG_URL, p.url, errMsg);

//Packager may be omitted, no need to check return code; 
  p.packager.erase();
  getStringTagValue(h, RPMTAG_PACKAGER, p.packager, errMsg);

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
	p.conflicts[i] = PkgRel(names[i], versions[i], flags[i]);
    }

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
	p.obsoletes[i] = PkgRel(names[i], versions[i], flags[i]);
    }

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

  for(PkgRelVector::size_type i = 0;i < p.provides.size();i++)
    translateRelFlags(p.provides[i]);
  for(PkgRelVector::size_type i = 0;i < p.requires.size();i++)
    translateRelFlags(p.requires[i]);
  for(PkgRelVector::size_type i = 0;i < p.conflicts.size();i++)
    translateRelFlags(p.conflicts[i]);
  for(PkgRelVector::size_type i = 0;i < p.obsoletes.size();i++)
    translateRelFlags(p.obsoletes[i]);
  headerFree(h);
  Fclose(fd);
  return 1;
}

bool printPackageInfo(const std::string& fileName)
{
  Package p;
  std::string errMsg;
  if (!readPackageData(fileName, p, errMsg))
    {
      std::cerr << "error:" << errMsg << std::endl;
	return 0;
    }
  std::cout << "# " << fileName << std::endl;
  std::cout << "name=" << p.name << std::endl;
  std::cout << "epoch=" << p.epoch << std::endl;
  std::cout << "version=" << p.version << std::endl;
  std::cout << "release=" << p.release << std::endl;
  std::cout << "arch=" << p.arch << std::endl;
  std::cout << "url=" << std::endl;
  std::cout << "packager=" << std::endl;
  std::cout << "summary=" << p.summary << std::endl;
  for (PkgRelVector::size_type i = 0;i < p.requires.size();i++)
    std::cout << "require:" << p.requires[i] << std::endl;
  for (PkgRelVector::size_type i = 0;i < p.provides.size();i++)
    std::cout << "provide:" << p.provides[i] << std::endl;
  for (PkgRelVector::size_type i = 0;i < p.conflicts.size();i++)
    std::cout << "conflict:" << p.conflicts[i] << std::endl;
  for (PkgRelVector::size_type i = 0;i < p.obsoletes.size();i++)
    std::cout << "obsolete:" << p.obsoletes[i] << std::endl;
  std::cout << std::endl;
  return 1;
}

int main(int argc, char* argv[])
{
  assert(argc >= 2);
  printPackageInfo(argv[1]);
  return 0;
}
