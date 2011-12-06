// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

// Note: custom allocators are not supported on VC6, since that compiler
// had trouble finding the function zlib_base::do_init.

//FIXME:typedef void* (*xalloc_func)(void*, zlib::uint, zlib::uint);
//FIXME:typedef void (*xfree_func)(void*, void*);

#ifndef DEPSOLVER_ZLIB_INTERFACE_H
#define DEPSOLVER_ZLIB_INTERFACE_H

#include<stdint.h>

typedef uint32_t ZLibUInt;
typedef uint8_t ZLibByte;
typedef uint32_t ZLibULong;

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

class ZLibException//FIXME:DepsolverException;
{
public:
  ZLibException(int error)
    : m_error(error) {}

public:
  int getCode() const
  { 
    return m_error; 
  }

private:
  int m_error;
}; //class ZLibException;

//Algorithms parameter structures;

struct ZLibParams 
{
  ZLibParams(int level = ZLibDefaultCompression,
	     int method = ZLibDeflated,
	     int windowBits  = ZLibDefaultWindowBits, 
	     int memLevel = ZLibDefaultMemLevel,
	     int strategy = ZLibDefaultStrategy,
	     bool noHeader = ZLibDefaultNoHeader,
	     bool calculateCrc = ZLibDefaultCRC)
    : level(level), 
      method(method), 
      windowBits(windowBits),
      memLevel(memLevel), 
      strategy(strategy), 
      noHeader(noHeader), 
      calculateCrc(calculateCrc) { }

  int level;
  int method;
  int windowBits;
  int memLevel;
  int strategy;
  bool noHeader;
  bool calculateCrc;
}; //class ZLibParams;

class ZLibBase
{
public:
  typedef char char_type;

protected:
  ZLibBase();
  ~ZLibBase();

protected:
  void* stream() 
  {
    return m_stream; 
  }

  void initDeflate(const ZLibParams& p);
  void initInflate(const ZLibParams& p);
  void controlErrorCode(int errorCode) const;

  void before(const char*& srcBegin, const char* srcEnd,
	      char*& destBegin, char* destEnd);

  void after(const char*& srcBegin, char*& destBegin, bool compress);

  int runDeflate(int flush);
  int runInflate(int flush);
  void reset(bool compress, bool realloc);

public:
  ZLibULong crc() const 
  { 
    return m_crc; 
  }

  int totalIn() const
  { 
    return m_totalIn;
  }

  int totalOut() const 
  { 
    return m_totalOut; 
  }

private:
  void* m_stream; // Actual type: z_stream*;
  bool m_calculateCrc;
  ZLibULong  m_crc, m_crcImp;
  int m_totalIn;
  int m_totalOut;
}; //class ZLibBase;

class ZLibCompressor: public ZLibBase
{
public: 
  ZLibCompressor(const ZLibParams& p = ZLibDefaultCompression)
  {
    initDeflate(p);
  }

  ~ZLibCompressor()
  { 
    ZLibBase::reset(1, 0);//1 means base was initialized for compression, 0 means we destroy object finally;
  }

public:
  bool filter(const char*& srcBegin, const char* srcEnd,
	      char*& destBegin, char* destEnd, bool flush)
  {
    before(srcBegin, srcEnd, destBegin, destEnd);
    const int result = runDeflate(flush?ZLibFinish :ZLibNoFlush);
    after(srcBegin, destBegin, 1);
    controlErrorCode(result);
    return result != ZLibStreamEnd; 
  }

  void reset()
  {
    ZLibBase::reset(1, 1);//first 1 means object was created for compression, the second 1 means we are reinitializing it;
  }
}; //class ZLibCompressor;

class ZLibDecompressor: public ZLibBase
{
public:
  ZLibDecompressor(const ZLibParams& p)
    : m_eof(0)
  {
    initInflate(p);
  }

  ZLibDecompressor(int windowBits = ZLibDefaultWindowBits)
    : m_eof(0)
  {
    ZLibParams p;
    p.windowBits = windowBits;
    initInflate(p);
  }

  ~ZLibDecompressor()
  {
    ZLibBase::reset(0, 0);//Zeroes mean the object was initialized for decompressing and must be closed finally;
  }

public:
  bool filter(const char*& beginIn, const char* endIn,
	      char*& beginOut, char* endOut, bool flush)
  {
    before(beginIn, endIn, beginOut, endOut);
    const int result = runInflate(ZLibSyncFlush);
    after(beginIn, beginOut, 0);
controlErrorCode(result);
    return !(m_eof = result == ZLibStreamEnd);
  }

  void reset()
  {
    m_eof = 0;
    ZLibBase::reset(0, 1);//Zero means the object was initialized for decompressing and must be reinitialized;
  }

  bool eof() const 
  {
    return m_eof; 
  }

private:
  bool m_eof;
}; //class ZLibDecompressor;

#endif //DEPSOLVER_COMPRESSION_STREAMS_H;
