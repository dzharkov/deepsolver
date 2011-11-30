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

void zlib_error::check BOOST_PREVENT_MACRO_SUBSTITUTION(int error)
{
  switch (error) {
  case Z_OK: 
  case Z_STREAM_END: 
    //case Z_BUF_ERROR: 
    return;
  case Z_MEM_ERROR: 
    boost::throw_exception(std::bad_alloc());
  default:
    boost::throw_exception(zlib_error(error));
    ;
  }
}

zlib_base::zlib_base()
  : stream_(new z_stream), calculate_crc_(false), crc_(0), crc_imp_(0)
{ 
}

zlib_base::~zlib_base() 
{
  delete static_cast<z_stream*>(stream_);
}

void zlib_base::before( const char*& src_begin, const char* src_end,
                        char*& dest_begin, char* dest_end )
{
  z_stream* s = static_cast<z_stream*>(stream_);
  s->next_in = reinterpret_cast<zlib::byte*>(const_cast<char*>(src_begin));
  s->avail_in = static_cast<zlib::uint>(src_end - src_begin);
  s->next_out = reinterpret_cast<zlib::byte*>(dest_begin);
  s->avail_out= static_cast<zlib::uint>(dest_end - dest_begin);
}

void zlib_base::after(const char*& src_begin, char*& dest_begin, bool compress)
{
  z_stream* s = static_cast<z_stream*>(stream_);
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

void zlib_base::do_init
( const zlib_params& p, bool compress, 
#if !BOOST_WORKAROUND(BOOST_MSVC, < 1300)
  zlib::xalloc_func /* alloc */, zlib::xfree_func /* free*/, 
#endif
  void* derived )
{
  calculate_crc_ = p.calculate_crc;
  z_stream* s = static_cast<z_stream*>(stream_);
  s->zalloc = 0;
  s->zfree = 0;
  //#endif
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

