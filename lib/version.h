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
bool versionOverlap(const VersionCond& ver1, const VersionCond& ver2);
bool versionSatisfies(const VersionCond& cond, Epoch epoch, const std::string& ver, const std::string& release);
bool versionSatisfies(const VersionCond& cond, const std::string& ver);
bool versionEqual(const std::string& ver1, const std::string& ver2);

/**\brief Checks if one version is newer than another
 *
 * \return /Non-zero if ver1 greater than ver2 or zero if ver1 less or equals ver2
 */
bool versionGreater(const std::string& ver1, const std::string& ver2);



#endif //DEEPSOLVER_VERSION_H;
