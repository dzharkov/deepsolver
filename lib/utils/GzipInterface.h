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

#ifndef DEEPSOLVER_GZIP_INTERFACE_H
#define DEEPSOLVER_GZIP_INTERFACE_H

class GzipOutputFile
{
public:
  GzipOutputFile()
    : m_fd(-1),
      m_gzfile(NULL) {}

  virtual ~GzipOutputFile()
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
}; //class GzipOutputFile;

class GzipInputFile
{
public:
  GzipInputFile()
    : m_fd(-1),
      m_gzfile(NULL) {}

  virtual ~GzipInputFile()
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
}; //class GzipOutputFile;

#endif //DEEPSOLVER_GZIP_INTERFACE_H;
