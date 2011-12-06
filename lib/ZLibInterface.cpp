// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

// To configure Boost to work with zlib, see the 
// installation instructions here:
// http://boost.org/libs/iostreams/doc/index.html?path=7

// Define BOOST_IOSTREAMS_SOURCE so that <boost/iostreams/detail/config.hpp> 
// knows that we are building the library (possibly exporting code), rather 
// than using it (possibly importing code).

#include"depsolver.h"
#include"ZLibInterface.h"
#include <zlib.h>   // Jean-loup Gailly's and Mark Adler's "zlib.h" header;

//ZLib constants;

const int ZLibNoCompression = Z_NO_COMPRESSION;
const int ZLibBestSpeed = Z_BEST_SPEED;
const int ZLibBestCompression = Z_BEST_COMPRESSION;
const int ZLibDefaultCompression = Z_DEFAULT_COMPRESSION;

const int ZLibDeflated = Z_DEFLATED;

const int ZLibDefaultStrategy = Z_DEFAULT_STRATEGY;
const int ZLibFiltered = Z_FILTERED;
const int ZLibHuffmanOnly = Z_HUFFMAN_ONLY;

const int ZLibOKay = Z_OK;
const int ZLibStreamEnd = Z_STREAM_END;
const int ZLibStreamError = Z_STREAM_ERROR;
const int ZLibVersionError  = Z_VERSION_ERROR;
const int ZLibDataError = Z_DATA_ERROR;
const int ZLibMemError = Z_MEM_ERROR;
const int ZLibBufError = Z_BUF_ERROR;

const int ZLibFinish  = Z_FINISH;
const int ZLibNoFlush = Z_NO_FLUSH;
const int ZLibSyncFlush = Z_SYNC_FLUSH;

//ZLibBase methods;

ZLibBase::ZLibBase()
  : m_stream(new z_stream), 
    m_calculateCrc(false),
    m_crc(0),
    m_crcImp(0)
{ 
  assert(m_stream);
  memset(m_stream, 0, sizeof(z_stream));
}

ZLibBase::~ZLibBase()
{
  delete static_cast<z_stream*>(m_stream);
}

void ZLibBase::initDeflate(const ZLibParams& p)
{
  m_calculateCrc = p.calculateCrc;
  z_stream* s = static_cast<z_stream*>(m_stream);
  assert(s);
  s->zalloc = Z_NULL;
  s->zfree = Z_NULL;
  s->opaque = Z_NULL;
  const int windowBits = p.noHeader? -p.windowBits : p.windowBits;
  const int errorCode = deflateInit2( s, p.level, p.method, windowBits, p.memLevel, p.strategy);
  controlErrorCode(errorCode);
}

void ZLibBase::initInflate(const ZLibParams& p)
{
  m_calculateCrc = p.calculateCrc;
  z_stream* s = static_cast<z_stream*>(m_stream);
  assert(s);
  s->zalloc = Z_NULL;
  s->zfree = Z_NULL;
  s->opaque = Z_NULL;
  const int windowBits = p.noHeader? -p.windowBits : p.windowBits;
  const int errorCode = inflateInit2(s, windowBits);
  controlErrorCode(errorCode);
}

void ZLibBase::controlErrorCode(int errorCode) const
{
  assert(errorCode == Z_OK || errorCode == Z_STREAM_END);
  /*FIXME:
  switch (error) 
    {
    case Z_OK: 
    case Z_STREAM_END: 
    //case Z_BUF_ERROR: 
      return;
  case Z_MEM_ERROR: 
    boost::throw_exception(std::bad_alloc());
    default:
      boost::throw_exception(zlib_error(error));
    }
  */
}

void ZLibBase::before(const char*& srcBegin, const char* srcEnd,
		      char*& destBegin, char* destEnd)
{
  z_stream* s = static_cast<z_stream*>(m_stream);
  assert(s);
  s->next_in = reinterpret_cast<ZLibByte*>(const_cast<char*>(srcBegin));
  s->avail_in = static_cast<ZLibUInt>(srcEnd - srcBegin);
  s->next_out = reinterpret_cast<ZLibByte*>(destBegin);
  s->avail_out= static_cast<ZLibUInt>(destEnd - destBegin);
}

void ZLibBase::after(const char*& srcBegin, char*& destBegin, bool compress)
{
  z_stream* s = static_cast<z_stream*>(m_stream);
  assert(s);
  char* nextIn = reinterpret_cast<char*>(s->next_in);
  char* nextOut = reinterpret_cast<char*>(s->next_out);
  assert(nextIn);
  assert(nextOut);
  if (m_calculateCrc) 
    {
    const ZLibByte* buf = compress ?
      reinterpret_cast<const ZLibByte*>(srcBegin) :
      reinterpret_cast<const ZLibByte*>(const_cast<const char*>(destBegin));
    const ZLibUInt length = compress ?
      static_cast<ZLibUInt>(nextIn - srcBegin) :
      static_cast<ZLibUInt>(nextOut - destBegin);
    if (length > 0)
      m_crc = m_crcImp = crc32(m_crcImp, buf, length);
    }
  m_totalIn = s->total_in;
  m_totalOut = s->total_out;
  srcBegin = const_cast<const char*>(nextIn);
  destBegin = nextOut;
}

int ZLibBase::runDeflate(int flush)
{ 
  z_stream* s = static_cast<z_stream*>(m_stream);
  assert(s);
  return ::deflate(s, flush);
}

int ZLibBase::runInflate(int flush)
{ 
  z_stream* s = static_cast<z_stream*>(m_stream);
  assert(s);
  return ::inflate(s, flush);
}

void ZLibBase::reset(bool compress, bool realloc)
{
  z_stream* s = static_cast<z_stream*>(m_stream);
  // Undiagnosed bug:
  // deflateReset(), etc., return Z_DATA_ERROR
  //zlib_error::check BOOST_PREVENT_MACRO_SUBSTITUTION(
  if (realloc )
    {
      if (compress)
	deflateReset(s); else
	inflateReset(s);
    } else 
    {
      if (compress)
	deflateEnd(s); else
	inflateEnd(s);
    }
  m_crcImp = 0;
}
