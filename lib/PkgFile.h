
#ifndef FIXME_PKG_FILE_H
#define FIXME_PKG_FILE_H

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

#endif //FIXME_PKG_FILE_H;
