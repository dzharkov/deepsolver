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

#ifndef DEEPSOLVER_REPO_INDEX_TEXT_FORMAT_READER_H
#define DEEPSOLVER_REPO_INDEX_TEXT_FORMAT_READER_H

#include"Pkg.h"
#include"utils/TextFiles.h"

class RepoIndexTextFormatReader
{
public:
  RepoIndexTextFormatReader(const std::string& dir, char compressionType)
    : m_dir(dir), m_compressionType(compressionType) {}

  ~RepoIndexTextFormatReader() {}

public:
  void openPackagesFile();
  void openSourcePackagesFile();
  void openProvidesFile();
  bool readPackage(PkgFile& pkgFile);
  bool readSourcePackage(PkgFile& pkgFile);
  bool readProvides(std::string& provideName, StringVector& providers);

private:
  const std::string& m_dir;
  const char m_compressionType;
  std::auto_ptr<AbstractTextFileReader> m_reader;
  std::string m_lastSectionHeader;
  bool m_noMoreData;
}; //class RepoIndexTextFormatReader;

#endif //DEEPSOLVER_REPO_INDEX_TEXT_FORMAT_READER_H;
