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

#ifndef DEEPSOLVER_PKG_SECTION_H
#define DEEPSOLVER_PKG_SECTION_H

#include"Pkg.h"

class PkgSection
{
public:
  static std::string saveBaseInfo(const PkgFile& pkgFile);
  static std::string saveDescr(const PkgFile& pkgFile, bool saveChangeLog);

private:
  static std::string encodeMultiline(const std::string& s);
  static std::string encodeChangeLogEntry(const ChangeLogEntry& entry);
  static std::string saveNamedPkgRel(const NamedPkgRel& r);
  static std::string saveFileName(const std::string& fileName);
  //FIXME:static std::string getPkgRelName(const std::string& line);
}; //class PkgSection;

#endif //DEEPSOLVER_PKG_SECTION_H;
