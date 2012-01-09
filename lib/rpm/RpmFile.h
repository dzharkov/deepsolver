
#ifndef DEPSOLVER_RPM_FILE_H
#define DEPSOLVER_RPM_FILE_H

#include"Pkg.h"
#include"RpmException.h"

bool readRpmPkgFile(const std::string& fileName, PkgFile& pkgFile, StringList& files);

#endif //DEPSOLVER_RPM_FILE_H;
