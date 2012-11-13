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

#ifndef DEEPSOLVER_RPM_VERCMP_H
#define DEEPSOLVER_RPM_VERCMP_H

int rpmVerCmp(const std::string& ver1, const std::string& ver2);

/**\brief Overlaps two version ranges
 *
 * This function is not symmetric. If second version range has no epoch
 * value it assumes the same as in first one if there any. So if this
 * function is used for requires processing the require entry should go
 * only as second argument.
 *
 * \param [in] v1 The first version range to intersect
 * \param [in] v2 The second version range to intersect
 *
 * \return Non-zero if intersection is not empty
 */
int rpmVerOverlap(const VersionCond& v1, const VersionCond& v2);

#endif //DEEPSOLVER_RPM_VERCMP_H;
