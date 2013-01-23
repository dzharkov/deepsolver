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

#ifndef DEEPSOLVER_TEXT_FORMAT_SECTION_READER_H
#define DEEPSOLVER_TEXT_FORMAT_SECTION_READER_H

#include"utils/GzipInterface.h"

class AbstractTextFormatSectionReader
{
public:
  AbstractTextFormatSectionReader() {}
  virtual ~AbstractTextFormatSectionReader() {}

public:
  void init();
  bool readNext(std::string& s);
  virtual void close() = 0;

protected:
  virtual size_t readData(void* buf, size_t bufSize) = 0;

private:
  bool m_noMoreData;
  std::string m_queue;
}; //class abstractTextFormatSectionReader; 

class TextFormatSectionReader: public AbstractTextFormatSectionReader
{
public:
  TextFormatSectionReader() {}
  virtual ~TextFormatSectionReader() {}

public:
  void open(const std::string& fileName)
  {
    m_file.open(fileName);
  }

  void close()
  {
    m_file.close();
  }

protected:
  size_t readData(void* buf, size_t bufSize)
  {
    return m_file.read(buf, bufSize);
  }

private:
  File m_file;
}; //class TextFormatSectionReader;

class TextFormatSectionReaderGzip: public AbstractTextFormatSectionReader
{
public:
  TextFormatSectionReaderGzip() {}
  virtual ~TextFormatSectionReaderGzip() {}

public:
  void open(const std::string& fileName)
  {
    m_file.open(fileName);
  }

  void close()
  {
    m_file.close();
  }

protected:
  size_t readData(void* buf, size_t bufSize)
  {
    return m_file.read(buf, bufSize);
  }

private:
  GzipInputFile m_file;
}; //class TextFormatSectionReader;

#endif //DEEPSOLVER_TEXT_FORMAT_SECTION_READER_H;
