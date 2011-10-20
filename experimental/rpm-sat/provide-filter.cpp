
#include<assert.h>
#include<iostream>
#include<fstream>
#include<set>

#define PREFIX "provide-filter:"

typedef std::set<std::string> StringSet;

bool stringBegins(const std::string& str, const std::string& headToCheck, std::string& tail)
{
  assert(!headToCheck.empty());
  if (str.length() < headToCheck.length() || str.find(headToCheck) != 0)
    return 0;
  tail = str.substr(headToCheck.length());
  return 1;
}

bool buildrequireSet(const std::string& fileName, StringSet& res)
{
  res.clear();
  std::ifstream s(fileName.c_str());
  if (!s)
    {
      std::cerr << PREFIX << "could not open \'" << fileName << "\' for reading" << std::endl;
	return 0;
    }
  while (1)
    {
      std::string line;
      std::getline(s, line);
      if (!s)
	return 1;
      if (line.empty() || line[0] == '#')
	continue;
      std::string tail;
      if (stringBegins(line, "require:", tail))
	{
	  const std::string::size_type space = tail.find(" ");
	  const std::string pkg = space != std::string::npos?tail.substr(0, space):tail;
	  res.insert(pkg);
	  continue;
	}
      if (stringBegins(line, "conflict:", tail))
	{
	  const std::string::size_type space = tail.find(" ");
	  const std::string pkg = space != std::string::npos?tail.substr(0, space):tail;
	  res.insert(pkg);
	  continue;
	}
      if (stringBegins(line, "obsolete:", tail))
	{
	  const std::string::size_type space = tail.find(" ");
	  const std::string pkg = space != std::string::npos?tail.substr(0, space):tail;
	  res.insert(pkg);
	  continue;
	}
    }
  assert(0);
  return 0;//Just to return warning messages;
}

bool filterProvideRecords(const std::string& inputFileName, const std::string& outputFileName, const StringSet& requires)
{
  std::ifstream is(inputFileName.c_str());
  if (!is)
    {
      std::cerr << PREFIX << "could not open \'" << inputFileName << "\' for reading" << std::endl;
	return 0;
    }
  std::ofstream os(outputFileName.c_str());
  if (!os)
    {
      std::cerr << PREFIX << "could not open \'" << outputFileName << "\' for writing" << std::endl;
	return 0;
    }
  while (1)
    {
      std::string line;
      std::getline(is, line);
      if (!is)
	return 1;
      if (line.empty() || line[0] == '#')
	{
	  os << line << std::endl;
	  continue;
	}
      std::string tail;
      if (stringBegins(line, "provide:", tail))
	{
	  const std::string::size_type space = tail.find(" ");
	  const std::string pkg = space != std::string::npos?tail.substr(0, space):tail;
	  if (requires.find(pkg) != requires.end())
	      os << line << std::endl;
	} else 
	    os << line << std::endl;
    }
  assert(0);
  return 0;//Just to return warning messages;
}

int main(int argc, char* argv[])
{
  assert(argc == 2);
  StringSet requires;
  if (!buildrequireSet(argv[1], requires))
    return 1;
  if (!filterProvideRecords(argv[1], std::string(argv[1]) + ".filtered", requires))
    return 1;
  return 0;
}
