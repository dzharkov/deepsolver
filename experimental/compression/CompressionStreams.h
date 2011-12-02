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
typedef uint32_t ZLibULong;

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

//GZip constants;

enum {
  GZipZLibError = 1,
  GZipBadCRC = 2, // Recorded crc doesn't match data;
  GZipBadLength = 3, // Recorded length doesn't match data;
  GZipBadHeader = 4, // Malformed header;
  GZipBadFooter = 5, // Malformed footer;
  GZipBadMethod = 6 // Unsupported compression method;
};

//Magic numbers used by gzip header;
const int GZipMagicID1 = 0x1f;
const int GZipMagicID2 = 0x8b;

//The code used for the 'CM' (compression method) byte of the gzip header;
enum {
  GZipMethodDeflate = 8
};

//Codes used for the 'FLG' byte of the gzip header;
enum {
  GZipFlagText = 1,
  GZipFlagHeaderCRC = 2,
  GZipFlagExtra = 4,
  GZipFlagName  = 8,
  GZipFlagComment = 16
};

// Codes used for the 'XFL' byte of the gzip header;
enum {
  GZipExtraFlagBestCompression = 2,
  GZipExtraFlagBestSpeed  = 4
};

// Codes used for the 'OS' byte of the gzip header;
enum {
  GZipOSFat = 0,
  GZipOSAmiga = 1,
  GZipOSVms = 2,
  GZipOSUnix = 3,
  GZipOSVmCms = 4,
  GZipOSAtari = 5,
  GZipOSHpFs = 6,
  GZipOSMacintosh = 7,
  GZipOSZSystem = 8,
  GZipOSCpM = 9,
  GZipOSTops20 = 10,
  GZipOSNtfs = 11,
  GZipOSQDos = 12,
  GZipOSAcorn = 13,
  GZipOSUnknown = 255
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

struct GZipParams: ZLibParams
{
  GZipParams(int level = GZipDefaultCompression,
	     int method = GZipDeflated,
	     int windowBits = GZipDefaultWindowBits,
	     int memLevel = GZipDefaultMemLevel,
	     int strategy = GZipDefaultStrategy,
	     std::string fileName = "",
	     std::string comment = "",
	     std::time_t mtime = 0)
    : ZLibParams(level, method, windowBits, memLevel, strategy),
      fileName(fileName),
      comment(comment),
      mtime(mtime) {}

  std::string  fileName;
  std::string  comment;
  std::time_t  mtime;
};

#endif //DEPSOLVER_COMPRESSION_STREAMS_H;

//ZLib classes;

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

  void init(const ZLibParams& p, bool compress);

  void before(const char*& srcBegin, const char* srcEnd,
	      char*& destBegin, char* destEnd);

  void after(const char*& srcBegin, char*& destBegin,
	     bool compress);

  int xdeflate(int flush);
  int xinflate(int flush);
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
  ZLibULong  m_crc;
  ZLibULong m_crcImp_;
  int m_totalIn;
  int m_totalOut;
}; //class ZLibBase;

//Here;

class ZLibCompressorImpl: public ZLibBase
{
public: 
  ZLibCompressorImpl(const ZLibParams& p = ZLibDefaultCompression)
  {
    init(p, 1);
  }

  ~ZLibCompressorImpl()
  { 
    reset(1, 0); 
  }

public:
  bool filter(const char*& srcBegin, const char* srcEnd,
	      char*& destBegin, char* destEnd, bool flush)
  {
    before(srcBegin, srcEnd, destBegin, destEnd);
    const int result = xdeflate(flush?ZLibFinish :ZLibNoFlush);
    after(srcBegin, destBegin, 1);
    ZLibError::check(result);
    return result != ZLibStreamEnd; 
  }

  void close()
  {
    reset(t1, 1);
  }
};

class ZLibDecompressorImpl: public ZLibBase
{
public:
  ZLibDecompressorImpl(const ZLibParams& p)
    : m_eof(0)
  {
    init(p, 0);
  }

  ZLibDecompressorImpl(int windowBits = ZLibDefaultWindowBits)
    : m_eof(0)
  {
    ZLibParams p;
    p.windowBits = windowBits;
    init(p, 0);
  }

  ~ZLibDecompressorImpl()
  {
    reset(0, 0);
  }

public:
  bool filter(const char*& beginIn, const char* endIn,
	      char*& beginOut, char* endOut, bool flush)
  {
    before(srcBegin, srcEnd, destBegin, destEnd);
    const int result = xinflate(ZLibSyncFlush);
    after(srcBegin, destBegin, 0);
    ZLibError::check(result);
    return !(m_eof = result == ZLibStreamEnd);
  }

  void close()
  {
    m_eof = 0;
    reset(0, 1);
  }

  bool eof() const 
  {
    return m_eof_; 
  }

private:
  bool m_eof;
};

