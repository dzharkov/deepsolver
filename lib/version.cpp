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

#include"deepsolver.h"
#include"version.h"
#include"rpm/vercmp.h"

int versionCompare(const std::string& ver1, const std::string& ver2)
{
  return rpmVerCmp(ver1, ver2);
}

bool versionOverlap(const VersionCond& ver1, const VersionCond& ver2)
{
  return rpmVerOverlap(ver1, ver2);
}

/*
bool versionSatisfies(const VersionCond& cond, Epoch epoch, const std::string& ver, const std::string& release)
{
  std::ostringstream ss;
  ss << epoch << ":" << ver << "-" << release;
  return versionSatisfies(cond, ss.str());
}

bool versionSatisfies(const VersionCond& cond, const std::string& ver)
{
  return versionOverlap(VersionCond(ver), cond);
}
*/

bool versionEqual(const std::string& ver1, const std::string& ver2)
{
  return versionOverlap(VersionCond(ver1), VersionCond(ver2));
}

bool versionGreater(const std::string& ver1, const std::string& ver2)
{
  return versionOverlap(VersionCond(ver1, VerLess), VersionCond(ver2));
}
