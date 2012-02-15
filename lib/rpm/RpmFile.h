
#ifndef DEEPSOLVER_RPM_FILE_H
#define DEEPSOLVER_RPM_FILE_H

#include"Pkg.h"
#include"RpmException.h"

bool readRpmPkgFile(const std::string& fileName, PkgFile& pkgFile, StringList& files);

#endif //DEEPSOLVER_RPM_FILE_H;
