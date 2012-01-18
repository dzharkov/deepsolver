

#ifndef DEPSOLVER_VERSION_H
#define DEPSOLVER_VERSION_H

bool versionSatisfies(const VersionCond& cond, Epoch epoch, const std::string& ver, const std::string& release);
bool versionSatisfies(const VersionCond& cond, const std::string& ver);

/**\brief Checks if one version is newer than another
 *
 * \return /Non-zero if ver1 greater than ver2
 */
bool versionGreater(const std::string& ver1, const std::string& ver2);



#endif //DEPSOLVER_VERSION_H;
