
#ifndef FIXME_RPM_FILE_H
#define FIXME_RPM_FILE_H

#include"PkgFile.h"
#include"NamesPkgRel.h"

bool readRpmFile(const std::string& fileName
		 PkgFile& pkgFile,
		 NamedPkgRelList& provides,
		 NamedPkgRelList& requires,
		 NamedPkgRelList& conflicts,
		 NamedPkgRelList& obsoletes);

#endif //FIXME_RPM_FILE_H;
