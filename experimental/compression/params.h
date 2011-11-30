// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

// Note: custom allocators are not supported on VC6, since that compiler
// had trouble finding the function zlib_base::do_init.

//FIXME:typedef void* (*xalloc_func)(void*, zlib::uint, zlib::uint);
//FIXME:typedef void (*xfree_func)(void*, void*);

struct GZipParams : ZLibParams 
{
  gzipParams( int level              = gzip::default_compression,
	       int method             = gzip::deflated,
	       int window_bits        = gzip::default_window_bits,
	       int mem_level          = gzip::default_mem_level,
	       int strategy           = gzip::default_strategy,
	       std::string file_name  = "",
	       std::string comment    = "",
	       std::time_t mtime      = 0 )
    : zlib_params(level, method, window_bits, mem_level, strategy),
      file_name(file_name), comment(comment), mtime(mtime)
  { }

    std::string  file_name;
  std::string  comment;
  std::time_t  mtime;
};

