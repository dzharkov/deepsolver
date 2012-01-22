

#ifndef DEPSOLVER_VERSION_H
#define DEPSOLVER_VERSION_H

#include"Pkg.h"

bool versionOverlap(const VersionCond& ver1, const VersionCond& ver2);
bool versionSatisfies(const VersionCond& cond, Epoch epoch, const std::string& ver, const std::string& release);
bool versionSatisfies(const VersionCond& cond, const std::string& ver);
bool versionEqual(const std::string& ver1, const std::string& ver2);

/**\brief Checks if one version is newer than another
 *
 * \return /Non-zero if ver1 greater than ver2 or zero if ver1 less or equals ver2
 */
bool versionGreater(const std::string& ver1, const std::string& ver2);



#endif //DEPSOLVER_VERSION_H;
