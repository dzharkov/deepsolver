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

#include"deepsolver.h"
#include"GzipInterface.h"
#include<zlib.h>

#define GZIP_STOP(x) throw GzipException(x)

void GzipOutputFile::open(const std::string& fileName)
{
  assert(m_fd == -1);
  m_fd = ::open(fileName.c_str(), O_CREAT | O_TRUNC | O_WRONLY, NEW_FILE_MODE);
  TRY_SYS_CALL(m_fd != -1, "open(" + fileName + ", O_CREAT | O_TRUNC | O_WRONLY)");
  m_gzfile = gzdopen(m_fd, "w");
  if (m_gzfile == NULL)
    {
      ::close(m_fd);
      m_fd = -1;
      GZIP_STOP("gzdfile(" + fileName + ") cannot create valid gzip file descriptor");
    }
  m_fileName = fileName;
}

void GzipOutputFile::write(const void* buf, size_t bufSize)
{
  if (bufSize == 0)
    return;
  assert(buf);
  assert(m_gzfile != NULL && m_fd != -1);
  gzFile f = static_cast<gzFile>(m_gzfile);
  size_t written = 0;
  const char* c = static_cast<const char*>(buf);
  while(written < bufSize)
    {
      const int res = gzwrite(f, &c[written], bufSize - written);
      if (res == 0)//the was an error;
	{
	  int code;
	  const char* msg = gzerror(f, &code);
	  if (code == Z_ERRNO)//The error is in the system, not in the zlib;
	    SYS_STOP("gzwrite(" + m_fileName + ")");
	  assert(msg);
	  GZIP_STOP(msg);
	}
      assert(res > 0);
      written += (size_t)res;
      assert(written <= bufSize);
    }
}

void GzipOutputFile::close()
{
  if (m_gzfile == NULL)
    return;
  assert(m_fd != -1);
  gzFile f = static_cast<gzFile>(m_gzfile);
  const int res = gzclose(f);
  if (res == Z_ERRNO)
    SYS_STOP("gzclose(" + m_fileName + ")");
  m_gzfile = NULL;
  m_fd = -1;//gzclose() automatically closes associated file descriptor;
  m_fileName.erase();
}

void GzipInputFile::open(const std::string& fileName)
{
  assert(m_fd == -1);
  m_fd = ::open(fileName.c_str(), O_RDONLY);
  TRY_SYS_CALL(m_fd != -1, "open(" + fileName + ", O_RDONLY)");
  m_gzfile = gzdopen(m_fd, "r");
  if (m_gzfile == NULL)
    {
      ::close(m_fd);
      m_fd = -1;
      GZIP_STOP("gzdfile(" + fileName + ") cannot create valid gzip file descriptor");
    }
  m_fileName = fileName;
}

size_t GzipInputFile::read(void* buf, size_t bufSize)
{
  if (bufSize == 0)
    return 0;
  assert(buf);
  assert(m_gzfile != NULL && m_fd != -1);
  gzFile f = static_cast<gzFile>(m_gzfile);
  const int res = gzread(f, buf, bufSize);
  if (res < 0)//There was an error;
    {
      int code;
      const char* msg = gzerror(f, &code);
      if (code == Z_ERRNO)//The error is in the system, not in the zlib;
	SYS_STOP("gzread(" + m_fileName + ")");
      assert(msg);
      GZIP_STOP(msg);
    }
return (size_t)res;
}

void GzipInputFile::close()
{
  if (m_gzfile == NULL)
    return;
  assert(m_fd != -1);
  gzFile f = static_cast<gzFile>(m_gzfile);
  const int res = gzclose(f);
  if (res == Z_ERRNO)
    SYS_STOP("gzclose(" + m_fileName + ")");
  m_gzfile = NULL;
  m_fd = -1;//gzclose() automatically closes associated file descriptor;
  m_fileName.erase();
}
