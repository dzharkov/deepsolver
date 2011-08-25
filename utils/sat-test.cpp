
#include<assert.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<time.h>
#include"Package.h"
#include"SATBase.h"
#include"SATFactory.h"
#include"SATOptimizer.h"

#define PREFIX "sat-test:"

bool parsePkgRel(const std::string& s, PkgRelVector& res)
{
  PkgRel p;
  std::string::size_type k = 0;
  while(k < s.length() && s[k] != ' ')
    {
    p.name += s[k];
    k++;
    }
  if (k >= s.length())
    return 0;
  while(k < s.length() && s[k] == ' ')
    k++;
  if (k >= s.length())
    return 0;
  if (s[k] == '<' || s[k] == '=' || s[k] == '>')
    {
      if (k + 1 >= s.length())
	return 0;
      std::string v;
      v += s[k];
      k++;
      if (s[k] != ' ')
	{
	  v += s[k];
	  k++;
	}
      if (k >= s.length() || s[k] != ' ')
	return 0;
      while (k < s.length() && s[k] == ' ')
	k++;
      if (k >= s.length())
	return 0;
      if (v == "<")
	p.versionRel = PkgRel::Less; else
      if (v == "<=")
	p.versionRel = PkgRel::LessOrEqual; else
      if (v == "=")
	p.versionRel = PkgRel::Equal; else
      if (v == ">")
	p.versionRel = PkgRel::Greater; else
      if (v == ">=")
	p.versionRel = PkgRel::GreaterOrEqual; else
	return 0;
      while(k < s.length() && s[k] != ' ')
	{
	  p.version += s[k];
	  k++;
	}
      if (k >= s.length())
	return 0;
      while(k < s.length() && s[k] == ' ')
	k++;
      if (k >= s.length())
	return 0;
    }
  assert(k < s.length());
  if (s[k] != '(')
    return 0;
  k++;
  std::string v;
  while(k < s.length() && s[k] != ')')
    {
      v += s[k];
      k++;
    }
  if (k >= s.length())
    return 0;
  std::istringstream is(v);
  if (!(is >> p.flags))
    return 0;
  res.push_back(p);
  return 1;
}

bool stringBegins(const std::string& str, const std::string& headToCheck, std::string& tail)
{
  assert(!headToCheck.empty());
  if (str.length() < headToCheck.length() || str.find(headToCheck) != 0)
    return 0;
  tail = str.substr(headToCheck.length());
  return 1;
}

bool loadPackageData(const std::string& fileName, PackageVector& packages)
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

      if (stringBegins(line, "require:", tail))
	{
	  if (!parsePkgRel(tail, p.requires))
	    {
	      std::cerr << PREFIX << "could not parse require string: \'" << tail << "\'" << std::endl;
	      return 0;
	    }
	}

      if (stringBegins(line, "provide:", tail))
	{
	  if (!parsePkgRel(tail, p.provides))
	    {
	      std::cerr << PREFIX << "could not parse provide string: \'" << tail << "\'" << std::endl;
	      return 0;
	    }
	}

      if (stringBegins(line, "conflict:", tail))
	{
	  if (!parsePkgRel(tail, p.conflicts))
	    {
	      std::cerr << PREFIX << "could not parse conflict string: \'" << tail << "\'" << std::endl;
	      return 0;
	    }
	}

      if (stringBegins(line, "obsolete:", tail))
	{
	  if (!parsePkgRel(tail, p.obsoletes))
	    {
	      std::cerr << PREFIX << "could not parse obsolete string: \'" << tail << "\'" << std::endl;
	      return 0;
	    }
	}
    }
  assert(0);
  return 1;//Just to reduce warning messages;
}

int main(int argc, char* argv[])
{
  assert(argc == 2);
  PackageVector packages;
  clock_t t1 = clock();
  double sec;
  if (!loadPackageData(argv[1], packages))
    return 1;
  sec = (double)(clock() - t1) / CLOCKS_PER_SEC;
  std::cout << "Loaded " << packages.size() << " packages in " << sec << " seconds" << std::endl;

  PackageId forPackage = 0;
  while (forPackage < packages.size() && packages[forPackage].name != "RHVoice")
    forPackage++;
  assert(forPackage < packages.size());

  SAT sat;
  t1 = clock();
  if (!fillSAT(packages, forPackage, sat))
    {
      std::cerr << PREFIX << "could not build SAT for " << packages[forPackage] << std::endl;
      return 1;
    }
  optimizeSAT(sat);
  sec = (double)(clock() - t1) / CLOCKS_PER_SEC;
  std::cout << "SAT was built in " << sec << " seconds" << std::endl;
  printSAT(std::cout, packages, sat);
  std::cout << std::endl;

  std::cout << "Done!!!" << std::endl;
  return 0;
}
