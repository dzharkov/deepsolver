
#ifndef DEPSOLVER_H
#define DEPSOLVER_H

#include"os/system.h"
#include"DefaultValues.h"
#include"logging.h"
#include"DepsolverException.h"
#include"os/SystemException.h"
#include"os/File.h"
#include"os/Directory.h"
#include"string-utils.h"

//Some basic types;

typedef size_t PackageId;
#define BAD_PACKAGE_ID (PackageId)-1
typedef std::vector<PackageId> PackageIdVector;
typedef std::list<PackageId> PackageIdList;

typedef size_t VarId;
#define BAD_VAR_ID (VarId)-1
typedef std::vector<VarId> VarIdVector;
typedef std::list<VarId> VarIdList;

#endif //DEPSOLVER_H
