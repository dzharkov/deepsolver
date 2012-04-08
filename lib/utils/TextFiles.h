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

#ifndef DEEPSOLVER_TEXT_FILES_H
#define DEEPSOLVER_TEXT_FILES_H

#include"DeepsolverException.h"

enum {
  TextFileStd = 0,
  TextFileGZip = 1
};

class TextFileException: public DeepsolverException
{
public:
  TextFileException(const std::string& msg)
    : m_msg(msg) {}

  virtual ~TextFileException() {}

public:
  std::string getType() const
  {
    return "textfile";
  }

  std::string getMessage() const
  {
    return m_msg;
  }

private:
  std::string m_msg;
}; //class TextFileException;

class AbstractTextFileWriter
{
public:
  virtual ~AbstractTextFileWriter() {}

public:
  virtual void writeLine(const std::string& line) = 0;
  virtual void close() = 0;
}; //class AbstractTextFileWriter;

class AbstractTextFileReader
{
public:
  virtual ~AbstractTextFileReader() {}

public:
  virtual bool readLine(std::string& line) = 0;
  virtual void close() = 0;
}; //class AbstractTextFileReader;

std::auto_ptr<AbstractTextFileReader> createTextFileReader(int type, const std::string& fileName);
std::auto_ptr<AbstractTextFileWriter> createTextFileWriter(int type, const std::string& fileName);

#endif //DEEPSOLVER_TEXT_FILES_H;
