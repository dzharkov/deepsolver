/*
   Copyright 2011-2012 ALT Linux
   Copyright 2011-2012 Michael Pozhidaev

   This file is part of the Deepsolver.

   Deepsolver is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   Deepsolver is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
*/

#ifndef DEEPSOLVER_VERSION_H
#define DEEPSOLVER_VERSION_H

#include"Pkg.h"

int versionCompare(const std::string& ver1, const std::string& ver2);

/**\brief Overlaps two version ranges
 *
 * This function is not symmetric. If second version range has no epoch
 * value it assumes the same as in first one if there any. So if this
 * function is used for requires processing the require entry should go
 * only as second argument.
 *
 * \param [in] ver1 The first version range to intersect
 * \param [in] ver2 The second version range to intersect
 *
 * \return Non-zero if intersection is not empty
 */
bool versionOverlap(const VersionCond& ver1, const VersionCond& ver2);

//bool versionSatisfies(const VersionCond& cond, Epoch epoch, const std::string& ver, const std::string& release);
//bool versionSatisfies(const VersionCond& cond, const std::string& ver);
bool versionEqual(const std::string& ver1, const std::string& ver2);

/**\brief Checks if one version is newer than another
 *
 * \return /Non-zero if ver1 greater than ver2 or zero if ver1 less or equals ver2
 */
bool versionGreater(const std::string& ver1, const std::string& ver2);

#endif //DEEPSOLVER_VERSION_H;
