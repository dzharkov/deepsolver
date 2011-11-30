// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

// Note: custom allocators are not supported on VC6, since that compiler
// had trouble finding the function zlib_base::do_init.

//FIXME:typedef void* (*xalloc_func)(void*, zlib::uint, zlib::uint);
//FIXME:typedef void (*xfree_func)(void*, void*);

#ifndef DEPSOLVER_COMPRESSION_STREAMS_H
#define DEPSOLVER_COMPRESSION_STREAMS_H

//General types;

typedef uint32_t ZLibUInt;
typedef uint8_t ZLibByte;
typedef uint32_t ZLibULongulong

//ZLib constants;

extern const int ZLibNoCompression;
extern const int ZLibBestSpeed;
extern const int BestCompression;
extern const int ZLibDefaultCompression;

extern const int ZLibDeflated;

extern const int ZLibDefaultStrategy;
extern const int ZLibFiltered;
extern const  int ZLibHuffmanOnly;

extern const int ZLibOKay;
extern const int ZLibStreamEnd;
extern const int ZLibStreamError;
extern const int ZLibVersionError;
extern const int ZLibDataError;
extern const int ZLibMemError;
extern const int ZLibBufError;

extern const int ZLibFinish;
extern const int ZLibNoFlush;
extern const int ZLibSyncFlush;

enum {
  ZLibDefaultWindowBits = 15,
  ZLibDefaultMemLevel = 8,
  ZLibDefaultCRC = 0,
  ZLibDefaultNoHeader  = 0
};

//Error information classes;

class ZLibError 
{
public:
  explicit ZLibError(int error)
    : m_error(error) {}

public:
  int getCode() const
  { 
    return m_error; 
  }

private:
  int m_error;
}; //class ZLibError;

class GZipError 
{
public:
  explicit GZipError(int error)
    : m_error(error), 
      m_zlibErrorCode(ZLibOKay) { }

  explicit GZipError(const ZLibError& e)
    : m_error(GZipZLibError), 
      m_zlibErrorCode(e.getCode()) {}

public:
  int getCode() const
  {
    return m_error;
  }

  int getZLibErrorCode() const 
  { 
    return m_ZLibErrorCode;
  }

private:
  int m_error;
  int m_ZLibErrorCode_;
};

//Algorithms parameter structures;

struct ZLibParams 
{
  ZLibParams(int level = ZLibDefaultCompression,
	     int method = ZLibDeflated,
	     int windowBits  = ZLibDefaultWindowBits, 
	     int memLevel = ZLibDefaultMemLevel,
	     int strategy = ZLibDefaultStrategy,
	     bool noHeader = ZLibDefaultNoHeader,
	     bool calculateCRC = ZLibDefaultCRC)
    : level(level), 
      method(method), 
      windowBits(windowBits),
      memLevel(memLevel), 
      strategy(strategy), 
      noHeader(noHeader), 
      calculateCRC(calculateCRC) { }

  int level;
  int method;
  int windowBits;
  int memLevel;
  int strategy;
  bool noHeader;
  bool calculateCRC;
};

#endif //DEPSOLVER_COMPRESSION_STREAMS_H;
