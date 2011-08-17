
#include<assert.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include"Package.h"

#define PREFIX "sat-test:"

bool stringBegins(const std::string& str, const std::string& headToCheck, std::string& tail)
{
  assert(!headToCheck.empty());
  if (str.length() < headToCheck.length() || str.find(headToCheck) != 0)
    return 0;
  tail = str.substr(headToCheck.length());
  return 1;
}

bool loadPackageData(const std::string& fileName, PackageList& packages)
{
  std::ifstream s(fileName.c_str());
  if (!s)
    {
      std::cerr << PREFIX << "could not open \'" << fileName << "\' for reading" << std::endl;
	return 0;
    }
  Package p;
  bool changed = 0;
  while (1)
    {
      std::string line;
      std::getline(s, line);
      if (!s)
	{
	  if (changed)
	    packages.push_back(p);
	  return 1;
	}
      if (line.empty())
	{
	  if (changed)
	    packages.push_back(p);
	  p = Package();
	  changed = 0;
	  continue;
	}
      assert(!line.empty());
      if (line[0] == '#')//This is a comment, silently do nothing;
	continue;

      changed = 1;
      std::string tail;

      if (stringBegins(line, "name=", tail))
	{
	  p.name = tail ;
	  continue;
	}

      if (stringBegins(line, "epoch=", tail))
	{
	  std::istringstream is(tail);
	  if (!(is >> p.epoch))
	    {
	      std::cerr << PREFIX << "invalid epoch value \'" << tail << "\'" << std::endl;
	      return 0;
	    }
	  continue;
	}

      if (stringBegins(line, "version=", tail))
	{
	  p.version = tail ;
	  continue;
	}

      if (stringBegins(line, "release=", tail))
	{
	  p.release = tail ;
	  continue;
	}

      if (stringBegins(line, "arch=", tail))
	{
	  p.arch = tail ;
	  continue;
	}

      if (stringBegins(line, "url=", tail))
	{
	  p.url = tail ;
	  continue;
	}

      if (stringBegins(line, "packager=", tail))
	{
	  p.packager = tail ;
	  continue;
	}

      if (stringBegins(line, "summary=", tail))
	{
	  p.summary = tail ;
	  continue;
	}

      //FIXME:requires, provides, conflicts and obsoletes;
    }
  assert(0);
  return 1;//Just to reduce warning messages;
}

int main(int argc, char* argv[])
{
  assert(argc == 2);
  PackageList packages;
  if (!loadPackageData(argv[1], packages))
    return 1;
  return 0;
}
