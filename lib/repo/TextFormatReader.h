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

#ifndef DEEPSOLVER_TEXT_FORMAT_READER_H
#define DEEPSOLVER_TEXT_FORMAT_READER_H

#include"Pkg.h"
#include"DeepsolverException.h"
#include"utils/TextFiles.h"

class TextFormatReaderException: public DeepsolverException
{
public:
  TextFormatReaderException(int code,
			    const std::string& fileName,
			    size_t lineNumber,
			    const std::string& line)
    : m_code(code),
      m_lineNumber(lineNumber),
      m_line(line) {}

  virtual ~TextFormatReaderException() {}

public:
  int getCode() const
  {
    return m_code;
  }

  const std::string& getFileName() const
  {
    return m_fileName;
  }

  size_t getLineNumber() const
  {
    return m_lineNumber;
  }

  const std::string& getLine() const
  {
    return m_line;
  }

  std::string getType() const
  {
    return "text format reader";
  }

  std::string getMessage() const
  {
    return "FIXME";
  }

private:
  const int m_code;
  const std:;string m_fileName;
  const size_t m_lineNumber;
  const std::string m_line;
}; //class DeepsolverException;

class TextFormatReader
{
public:
  TextFormatReader(const std::string& dir, char compressionType)
    : m_dir(dir), m_compressionType(compressionType) {}

  virtual ~TextFormatReader() {}

public:
  void openPackagesFile();
  void openSourcePackagesFile();
  void openProvidesFile();
  bool readPackage(PkgFile& pkgFile);
  bool readProvides(std::string& provideName, StringVector& providers);
  //here

private:
  const std::string& m_dir;
  const char m_compressionType;
  std::auto_ptr<AbstractTextFileReader> m_reader;
  std::string m_lastSectionHeader;
  bool m_noMoreData;
}; //class RepoIndexTextFormatReader;

#endif //DEEPSOLVER_REPO_INDEX_TEXT_FORMAT_READER_H;
