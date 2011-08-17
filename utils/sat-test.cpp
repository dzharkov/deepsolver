
#include<assert.h>
#include<iostream>
#include<fstream>
#include"Package.h"

#define PREFIX "sat-test:"

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
      return 1;
      std::cout << line << std::endl;
    }
  assert(0);
  return 1;//Just to reduce warning messages;
}

int main(int argc, char* argv[])
{
  assert(argc == 2);
  PackageVector packages;
  if (!loadPackageData(argv[1], packages))
    return 1;
  return 0;
}
