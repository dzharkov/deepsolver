/*
   Copyright 2011-2013 ALT Linux
   Copyright 2011-2013 Michael Pozhidaev

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
  static std::string saveBaseInfo(const PkgFile& pkgFile, const StringVector& filterProvidesByDirs);
  static std::string saveDescr(const PkgFile& pkgFile, bool saveChangeLog);
  static std::string saveFileList(const PkgFile& pkgFile);
  static bool isProvidesLine(const std::string& line, std::string& pkgName);
  static std::string getPkgFileName(const std::string& section);
  static void extractProvidesReferences(const std::string& section, StringSet& refs);
  static bool parsePkgFileSection(const StringVector& sect, PkgFile& pkgFile, size_t& invalidLineNum, std::string& invalidLineValue);
}; //class PkgSection;

#endif //DEEPSOLVER_PKG_SECTION_H;
