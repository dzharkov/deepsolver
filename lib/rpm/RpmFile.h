
#ifndef FIXME_RPM_FILE_H
#define FIXME_RPM_FILE_H

#include"PkgFile.h"
#include"NamesPkgRel.h"
#include"RpmException.h"

bool readRpmPkgFile(const std::string& fileName
		 PkgFile& pkgFile,
		 NamedPkgRelList& provides,
		 NamedPkgRelList& requires,
		 NamedPkgRelList& conflicts,
		 NamedPkgRelList& obsoletes);

#endif //FIXME_RPM_FILE_H;
