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

#ifndef DEEPSOLVER_REPO_PARAMS_H
#define DEEPSOLVER_REPO_PARAMS_H

class RepoParams
{
public:
  enum {
    CompressionTypeNone = 0,
    CompressionTypeGzip = 1
  };

  enum {
    FormatTypeText = 1,//The single currently supported;
    FormatTypeBinary = 2
  };

  RepoParams()
    : compressionType(CompressionTypeNone),
      formatType(FormatTypeText),
      filterProvidesByRefs(0),
      changeLogBinary(0),
      changeLogSources(0)
  {}

public:
  char compressionType;
  char formatType;
  //If the following list is empty provide filtering by directories is disabled;
  StringVector filterProvideByDirs;
  bool filterProvidesByRefs;
  StringVector excludeRequiresRegExp;
  bool changeLogBinary;
  bool changeLogSources;
  StringToStringMap userParams;

  //Values not mapped into repo information file;
  std::string indexPath;
  StringVector pkgSources;
  StringVector providesRefsSources;

public:
  void writeInfoFile(const std::string& fileName) const;
}; //class RepoParams;

#endif //DEEPSOLVER_REPO_PARAMS_H;
