
#ifndef DEPSOLVER_PKG_H
#define DEPSOLVER_PKG_H

#include"DateTime.h"

typedef size_t PackageId;
#define BAD_PACKAGE_ID (PackageId)-1

typedef unsigned short Epoch;

class Pkg
{
public:
  Pkg()
    : epoch(0), buildTime(0)  {}
  virtual ~Pkg() {}

public:
  Epoch epoch;
  std::string name;
  std::string version;
  std::string release;
  std::string arch;
  std::string packager;
  std::string url;
  std::string license;
  std::string srcRpm;
  std::string summary;
  std::string description;
  time_t buildTime;
}; //class Pkg;

#endif //DEPSOLVER_PKG_H;
