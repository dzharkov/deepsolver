// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef DEPSOLVER_ZLIB_INTERFACE_H
#define DEPSOLVER_ZLIB_INTERFACE_H

#include"DepsolverException.h"

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

class ZLibException: public DepsolverException
{
public:
  ZLibException(int code)
    : m_code(code) {}

public:
  int getCode() const
  { 
    return m_code;
  }

  std::string getType() const
  {
    return "zlib";
  }

  std::string getMessage() const
  {
    assert(m_code != ZLibOKay);//The valid code but not an error;
    if (m_code == ZLibStreamEnd)
      return "unexpected end of stream";
    if (m_code == ZLibStreamError)
      return "stream problem";
    if (m_code == ZLibVersionError)
      return "unsupported data version";
    if (m_code == ZLibDataError)
      return "broken data";
    if (m_code == ZLibMemError)
      return "no enough memory for allocation";
    if (m_code == ZLibBufError)
      return "buffer problem";
    assert(0);
    return "";//Just to reduce warning messages;
  }

private:
  int m_code;
}; //class ZLibException;

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

  void before(const char* srcBegin, size_t srcLength,
	      char* destBegin, size_t destLength);

  void after(const char** srcBegin, char** destBegin, bool compress);

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
    : m_srcPos(NULL),
      m_destPos(NULL),
      m_srcProcessed(0),
      m_destProcessed(0)
  {
    initDeflate(p);
  }

  ~ZLibCompressor()
  { 
    ZLibBase::reset(1, 0);//1 means base was initialized for compression, 0 means we destroy object finally;
  }

public:
  bool filter(const char* srcBegin, size_t srcLength,
	      char* destBegin, size_t destLength, bool flush)
  {
    before(srcBegin, srcLength, destBegin, destLength);
    const int result = runDeflate(flush?ZLibFinish :ZLibNoFlush);
    after(&m_srcPos, &m_destPos, 1);//1 means compressing mode;
    assert(m_srcPos);
    assert(m_destPos);
    controlErrorCode(result);
    m_srcProcessed = static_cast<size_t>(m_srcPos - srcBegin);
    m_destProcessed = static_cast<size_t>(m_destPos - destBegin);
    return result != ZLibStreamEnd; 
  }

  void reset()
  {
    ZLibBase::reset(1, 1);//first 1 means object was created for compression, the second 1 means we are reinitializing it;
  }

  const char* getSrcPos() const
  {
    assert(m_srcPos);
    return m_srcPos;
  }

  char* getDestPos()
  {
    assert(m_destPos);
    return m_destPos;
  }

  size_t getSrcProcessed() const
  {
    return m_srcProcessed;
  }

  size_t getDestProcessed() const
  {
    return m_destProcessed;
  }

private:
  const char* m_srcPos;
  char* m_destPos;
  size_t m_srcProcessed, m_destProcessed;
}; //class ZLibCompressor;

class ZLibDecompressor: public ZLibBase
{
public:
  ZLibDecompressor(const ZLibParams& p)
    : m_eof(0),
      m_srcPos(0),
      m_destPos(0),
      m_srcProcessed(0),
      m_destProcessed(0)
  {
    initInflate(p);
  }

  ZLibDecompressor(int windowBits = ZLibDefaultWindowBits)
    : m_eof(0),
      m_srcPos(0),
      m_destPos(0),
      m_srcProcessed(0),
      m_destProcessed(0)
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
  bool filter(const char* srcBegin, size_t srcLength,
	      char* destBegin, size_t destLength)
  {
    assert(srcLength > 0);
    before(srcBegin, srcLength, destBegin, destLength);
    const int result = runInflate(ZLibSyncFlush);
    after(&m_srcPos, &m_destPos, 0);//0 means decompressing;
controlErrorCode(result);
    m_srcProcessed = static_cast<size_t>(m_srcPos - srcBegin);
    m_destProcessed = static_cast<size_t>(m_destPos - destBegin);
    m_eof = result == ZLibStreamEnd;
    return !m_eof;
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

  const char* getSrcPos() const
  {
    assert(m_srcPos);
    return m_srcPos;
  }

  char* getDestPos()
  {
    assert(m_destPos);
    return m_destPos;
  }

  size_t getSrcProcessed() const
  {
    return m_srcProcessed;
  }

  size_t getDestProcessed() const
  {
    return m_destProcessed;
  }

private:
  bool m_eof;
  const char* m_srcPos;
  char* m_destPos;
  size_t m_srcProcessed, m_destProcessed;
}; //class ZLibDecompressor;

#endif //DEPSOLVER_COMPRESSION_STREAMS_H;
