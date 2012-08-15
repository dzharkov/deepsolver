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

enum {
  TextFormatReaderErrorInvalidSectionHeader = 0
};

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
    std::ostringstream ss;
    switch (m_code)
      {
      case TextFormatReaderErrorInvalidSectionHeader:
	ss << "Invalid section header";
	break;
      default:
	assert(0);
      }
    ss << ":" << m_fileName << "(" << m_lineNumber << "):" << m_line;
    return ss.str();
  }

private:
  const int m_code;
  const std::string m_fileName;
  const size_t m_lineNumber;
  const std::string m_line;
}; //class DeepsolverException;

class TextFormatReader
{
public:
  TextFormatReader() 
    : m_noMoreData(0), m_lineNumber(0) {}

  virtual ~TextFormatReader() {}

public:
  void openFile(const std::string& fileName, int textFileType);
  void close();
  bool readPackage(PkgFile& pkgFile);

private:
  void translateRelType(const std::string& str, NamedPkgRel& rel);
  void parsePkgRel(const std::string& str, NamedPkgRel& rel);
  void parsePkgFileSection(const StringList& sect, PkgFile& pkgFile);
  bool readLine(std::string& line);

private:
  //FIXME:  std::auto_ptr<AbstractTextFileReader> m_reader;
  std::string m_lastSectionHeader;
  bool m_noMoreData;
  //The following fields are used for informative exception throwing;
  std::string m_fileName;
  size_t m_lineNumber;
  std::string m_line;
}; //class RepoIndexTextFormatReader;

#endif //DEEPSOLVER_REPO_INDEX_TEXT_FORMAT_READER_H;
