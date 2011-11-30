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
