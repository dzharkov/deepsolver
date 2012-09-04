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
  static std::string saveBaseInfo(const PkgFile& pkgFile, const StringVector& filterProvidesByDirs);
  static std::string saveDescr(const PkgFile& pkgFile, bool saveChangeLog);
  static std::string saveFileList(const PkgFile& pkgFile);
  static bool isProvidesLine(const std::string& line, std::string& pkgName);
  static std::string getPkgFileName(const std::string& section);
  PkgSection::extractProvidesReferences(sect, references);//FIXME:

private:
  static std::string encodeMultiline(const std::string& s);
  static std::string encodeChangeLogEntry(const ChangeLogEntry& entry);
  static std::string saveNamedPkgRel(const NamedPkgRel& r);
  static std::string saveFileName(const std::string& fileName);
  static bool fileFromDirs(const std::string& fileName, const StringVector& dirs);
  static std::string extractPkgRelName(const std::string& line);
}; //class PkgSection;

#endif //DEEPSOLVER_PKG_SECTION_H;
