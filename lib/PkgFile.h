
#ifndef DEPSOLVER_PKG_FILE_H
#define DEPSOLVER_PKG_FILE_H

#include"Pkg.h"

class PkgFile: public Pkg
{
public:
  PkgFile()
    : source(0)  {}

  virtual ~PkgFile() {}

public:
  std::string fileName;
  bool source;
}; //class PkgFile;

#endif //DEPSOLVER_PKG_FILE_H;
