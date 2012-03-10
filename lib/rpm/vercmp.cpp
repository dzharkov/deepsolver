/*
   Copyright 2011-2012 ALT Linux
   Copyright 2011-2012 Dmitry V. Levin
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

#include"deepsolver.h"
#include<rpm/rpmlib.h>

int rpmVerCmp(const std::string& ver1, const std::string& ver2)
{
  return rpmvercmp(ver1.c_str(), ver2.c_str());
}

static int buildSenseFlags(const VersionCond& c)
{
  int value = 0;
  if (c.isEqual())
    value |= RPMSENSE_EQUAL;
  if (c.isLess())
    value |= RPMSENSE_LESS;
  if (c.isGreater())
    value |= RPMSENSE_GREATER;
  return value;
}

int rpmVerOverlap(const VersionCond& v1, const VersionCond& v2)
{
  return rpmRangesOverlap("", v1.version.c_str(), buildSenseFlags(v1),
			  "", v2.version.c_str(), buildSenseFlags(v2));
}
