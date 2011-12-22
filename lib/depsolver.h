
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

/*
 * PackageId type is the identifier of an item from the set built by
 * included strings from set of package names and set of all their
 * provides without any additional information such as version or anything
 * else.  Set of packages to consider consists of three parts: packages
 * from accessible repositories, installed packages in the system and
 * packages provided by user as files in the file system or by URL (may
 * absent in accessible repositories). 
 *
 * This type is basically  used due to performance resons.
 */
typedef size_t PackageId;
#define BAD_PACKAGE_ID (PackageId)-1
typedef std::vector<PackageId> PackageIdVector;
typedef std::list<PackageId> PackageIdList;

/*
 * The VarId type is used to identify one exact package from the set of
 * all known packages. The set of all known packages consists of all
 * packages from the accessible repositories, all installed packages and
 * packages provided by user as files in file system or by URL. Two or
 * more packages can be identify by the same VarId if they have the same
 * name, the same epoch, the same version, the same release and the same
 * build time. In all other cases packages must be considered as
 * different packages and must be identified by different values of
 * VarId. In this meaning values of VarId can be used as variables in SAT
 * formulas.
 *
 * The VarId must not be confused with PackageId type.
 */
typedef size_t VarId;
#define BAD_VAR_ID (VarId)-1
typedef std::vector<VarId> VarIdVector;
typedef std::list<VarId> VarIdList;

#endif //DEPSOLVER_H
