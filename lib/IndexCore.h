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

#ifndef DEEPSOLVER_INDEX_CORE_H
#define DEEPSOLVER_INDEX_CORE_H

#include"AbstractConsoleMessages.h"
#include"AbstractWarningHandler.h"
#include"IndexCoreException.h"

class RepoIndexParams
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

  RepoIndexParams()
    : compressionType(DEFAULT_REPO_INDEX_COMPRESSION_TYPE), //Real value is defined in DefaultValues.h;
      formatType(DEFAULT_REPO_INDEX_FORMAT_TYPE), //Real value is defined in DefaultValues.h;
    provideFilteringByRefs(0),
    changeLogBinary(0),
      changeLogSources(0)
  {}

public:
  char compressionType;//Must be exactly signed for error value indications;
  char formatType;//Must be exactly signed for error value indications;
  std::string arch;
  std::string topDir;
  StringToStringMap userParams;
  //If the following list is empty provide filtering by directories is disabled;
  StringList provideFilterDirs;
  bool provideFilteringByRefs;
  StringList takeRefsFromPackageDirs;
  bool changeLogBinary;
  std::string excludeRequiresFile;
  bool changeLogSources;
}; //class RepoIndexParams;

class IndexCore
{
public:
  IndexCore(AbstractConsoleMessages& console, AbstractWarningHandler& warningHandler)
    : m_console(console), m_warningHandler(warningHandler) {}

public:
  void build(const RepoIndexParams& params);

private:
  void collectRefs(const std::string& dirName, StringSet& res);
  void collectRefsFromDirs(const StringList& dirs, StringSet& res);
  void processPackages(const std::string& indexDir, const std::string& rpmsDir, const std::string& srpmsDir, const RepoIndexParams& params);
  void writeInfoFile(const std::string& fileName, const RepoIndexParams& params);

private:
  AbstractConsoleMessages& m_console;
  AbstractWarningHandler& m_warningHandler;
}; //class IndexCore;

#endif //DEEPSOLVER_INDEX_CORE_H;
