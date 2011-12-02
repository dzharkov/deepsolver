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

//FIXME:#include"depsolver.h"
#include"CompressionStreams.h"
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

//ZLibError;

//FIXME:
/*
void ZLibError::check(int error)
{
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
}
*/

//ZLibBase;

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

void ZLibBase::init(const ZLibParams& p, bool compress, void* derived)
{
  //FIXME:general clean up;
  calculate_crc_ = p.calculate_crc;
  z_stream* s = static_cast<z_stream*>(m_stream);
  assert(s);
  s->zalloc = 0;
  s->zfree = 0;
  s->opaque = derived;
  int window_bits = p.noheader? -p.window_bits : p.window_bits;
  zlib_error::check BOOST_PREVENT_MACRO_SUBSTITUTION(
						     compress ?
						     deflateInit2( s, 
								   p.level,
								   p.method,
								   window_bits,
								   p.mem_level,
								   p.strategy ) :
						     inflateInit2(s, window_bits)
						     );
}

void ZLibBase::before( const char*& src_begin, const char* src_end,
		       char*& dest_begin, char* dest_end )
{
  z_stream* s = static_cast<z_stream*>(m_stream);
  s->next_in = reinterpret_cast<zlib::byte*>(const_cast<char*>(src_begin));
  s->avail_in = static_cast<zlib::uint>(src_end - src_begin);
  s->next_out = reinterpret_cast<zlib::byte*>(dest_begin);
  s->avail_out= static_cast<zlib::uint>(dest_end - dest_begin);
}

void ZLibBase::after(const char*& src_begin, char*& dest_begin, bool compress)
{
  z_stream* s = static_cast<z_stream*>(m_stream);
  char* next_in = reinterpret_cast<char*>(s->next_in);
  char* next_out = reinterpret_cast<char*>(s->next_out);
  if (calculate_crc_) {
    const zlib::byte* buf = compress ?
      reinterpret_cast<const zlib::byte*>(src_begin) :
      reinterpret_cast<const zlib::byte*>(
					  const_cast<const char*>(dest_begin)
					  );
    zlib::uint length = compress ?
      static_cast<zlib::uint>(next_in - src_begin) :
      static_cast<zlib::uint>(next_out - dest_begin);
    if (length > 0)
      crc_ = crc_imp_ = crc32(crc_imp_, buf, length);
  }
  total_in_ = s->total_in;
  total_out_ = s->total_out;
  src_begin = const_cast<const char*>(next_in);
  dest_begin = next_out;
}

int zlib_base::xdeflate(int flush)
{ 
  return ::deflate(static_cast<z_stream*>(stream_), flush);
}

int zlib_base::xinflate(int flush)
{ 
  return ::inflate(static_cast<z_stream*>(stream_), flush);
}

void zlib_base::reset(bool compress, bool realloc)
{
  z_stream* s = static_cast<z_stream*>(stream_);
  // Undiagnosed bug:
  // deflateReset(), etc., return Z_DATA_ERROR
  //zlib_error::check BOOST_PREVENT_MACRO_SUBSTITUTION(
  realloc ?
    (compress ? deflateReset(s) : inflateReset(s)) :
    (compress ? deflateEnd(s) : inflateEnd(s));
    //);
  crc_imp_ = 0;
}

//GZipHeader;

void gzip_header::process(char c)
{
  uint8_t value = static_cast<uint8_t>(c);
  switch (state_) {
  case s_id1:
    if (value != gzip::magic::id1)
      boost::throw_exception(gzip_error(gzip::bad_header));
    state_ = s_id2;
    break;
  case s_id2:
    if (value != gzip::magic::id2)
      boost::throw_exception(gzip_error(gzip::bad_header));
    state_ = s_cm;
    break;
  case s_cm:
    if (value != gzip::method::deflate)
      boost::throw_exception(gzip_error(gzip::bad_method));
    state_ = s_flg;
    break;
  case s_flg:
    flags_ = value;
    state_ = s_mtime;
    break;
  case s_mtime:
    mtime_ += value << (offset_ * 8);
    if (offset_ == 3) {
      state_ = s_xfl;
      offset_ = 0;
    } else {
      ++offset_;
    }
    break;
  case s_xfl:
    state_ = s_os;
    break;
  case s_os:
    os_ = value;
    if (flags_ & gzip::flags::extra) {
      state_ = s_extra;
    } else if (flags_ & gzip::flags::name) {
      state_ = s_name;
    } else if (flags_ & gzip::flags::comment) {
      state_ = s_comment;
    } else if (flags_ & gzip::flags::header_crc) {
      state_ = s_hcrc;
    } else {
      state_ = s_done;
    }
    break;
  case s_xlen:
    xlen_ += value << (offset_ * 8);
    if (offset_ == 1) {
      state_ = s_extra;
      offset_ = 0;
    } else {
      ++offset_;
    }
    break;
  case s_extra:
    if (--xlen_ == 0) {
      if (flags_ & gzip::flags::name) {
	state_ = s_name;
      } else if (flags_ & gzip::flags::comment) {
	state_ = s_comment;
      } else if (flags_ & gzip::flags::header_crc) {
	state_ = s_hcrc;
      } else {
	state_ = s_done;
      }
    }
    break;
  case s_name:
    if (c != 0) {
      file_name_ += c;
    } else if (flags_ & gzip::flags::comment) {
      state_ = s_comment;
    } else if (flags_ & gzip::flags::header_crc) {
      state_ = s_hcrc;
    } else {
      state_ = s_done;
    }
    break;
  case s_comment:
    if (c != 0) {
      comment_ += c;
    } else if (flags_ & gzip::flags::header_crc) {
      state_ = s_hcrc;
    } else {
      state_ = s_done;
    }
    break;
  case s_hcrc:
    if (offset_ == 1) {
      state_ = s_done;
      offset_ = 0;
    } else {
      ++offset_;
    }
    break;
  default:
    BOOST_ASSERT(0);
  }
}

void gzip_header::reset()
{
  file_name_.clear();
  comment_.clear();
  os_ = flags_ = offset_ = xlen_ = 0;
  mtime_ = 0;
  state_ = s_id1;
}

void gzip_footer::process(char c)
{
  uint8_t value = static_cast<uint8_t>(c);
  if (state_ == s_crc) {
    crc_ += value << (offset_ * 8);
    if (offset_ == 3) {
      state_ = s_isize;
      offset_ = 0;
    } else {
      ++offset_;
    }
  } else if (state_ == s_isize) {
    isize_ += value << (offset_ * 8);
    if (offset_ == 3) {
      state_ = s_done;
      offset_ = 0;
    } else {
      ++offset_;
    }
  } else {
    BOOST_ASSERT(0);
  }
}

void gzip_footer::reset()
{
  crc_ = isize_ = offset_ = 0;
  state_ = s_crc;
}


