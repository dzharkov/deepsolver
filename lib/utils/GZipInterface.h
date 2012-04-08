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

#ifndef DEEPSOLVER_GZIP_INTERFACE_H
#define DEEPSOLVER_GZIP_INTERFACE_H

#include"DeepsolverException.h"

class GZipException: public DeepsolverException
{
public:
  GZipException(const std::string& msg)
    : m_msg(msg) {}

  virtual ~GZipException() {}

public:
  std::string getType() const
  {
    return "gzip";
  }

  std::string getMessage() const
  {
    return m_msg;
  }

private:
  const std::string m_msg;
}; //class GZipException;

class GZipOutputFile
{
public:
  GZipOutputFile()
    : m_fd(-1),
      m_gzfile(NULL) {}

  ~GZipOutputFile()
  {
    close();
  }

public:
  void open(const std::string& fileName);
  void write(const void* buf, size_t bufSize);
  void close();

private:
  int m_fd;
  void* m_gzfile;
  std::string m_fileName;
}; //class GZipOutputFile;

class GZipInputFile
{
public:
  GZipInputFile()
    : m_fd(-1),
      m_gzfile(NULL) {}

  ~GZipInputFile()
  {
    close();
  }

public:
  void open(const std::string& fileName);
  //The method below returns less data than requested block size in case of end of file;
  size_t read(void* buf, size_t bufSize);
  void close();

private:
  int m_fd;
  void* m_gzfile;
  std::string m_fileName;
}; //class GZipOutputFile;

#endif //DEEPSOLVER_GZIP_INTERFACE_H;
