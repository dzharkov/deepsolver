
#ifndef DEPSOLVER_RPM_FILE_H
#define DEPSOLVER_RPM_FILE_H

#include"PkgFile.h"
#include"NamedPkgRel.h"
#include"ChangeLog.h"
#include"RpmException.h"

bool readRpmPkgFile(const std::string& fileName,
		 PkgFile& pkgFile,
		 NamedPkgRelList& provides,
		 NamedPkgRelList& requires,
		 NamedPkgRelList& conflicts,
		    NamedPkgRelList& obsoletes,
		    StringList& files,
		    ChangeLog& changeLog);

#endif //DEPSOLVER_RPM_FILE_H;
