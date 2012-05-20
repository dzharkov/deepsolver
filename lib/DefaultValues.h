/*
   Copyright 2011-2012 ALT Linux
   Copyright 2011-2012 Michael Pozhidaev

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

#ifndef DEEPSOLVER_DEFAULT_VALUES_H
#define DEEPSOLVER_DEFAULT_VALUES_H

//Configuration parameters;
#define DEFAULT_CONFIG_DIR "/etc/deepsolver"
#define DEFAULT_CONFIG_FILE_NAME "ds.ini"

//Data files and directories;
#define COMPRESSION_SUFFIX_GZIP ".gz"
#define PKG_DATA_FILE_NAME "deepsolver-pkgs.bin"
#define PKG_DATA_FETCH_DIR "__tmp_pkg_data"

//Repo structure;
#define REPO_RPMS_DIR_NAME "RPMS"
#define REPO_SRPMS_DIR_NAME "SRPMS"

//Repo index parameters;
#define REPO_INDEX_DIR "base" //KILLME:temporary for compatibility;
#define REPO_INDEX_DIR_PREFIX "base."
#define REPO_INDEX_INFO_FILE "info"
#define INFO_FILE_FORMAT_TYPE "format_type"
#define INFO_FILE_COMPRESSION_TYPE "compression_type"
#define INFO_FILE_MD5SUM "md5sum_file"
#define INFO_FILE_FORMAT_TYPE_TEXT "text"
#define INFO_FILE_FORMAT_TYPE_BINARY "binary"
#define INFO_FILE_COMPRESSION_TYPE_NONE "none"
#define INFO_FILE_COMPRESSION_TYPE_GZIP "gzip"
#define REPO_INDEX_PACKAGES_DATA_FILE "rpms.data"
#define REPO_INDEX_SOURCE_DATA_FILE "srpms.data"
#define REPO_INDEX_PROVIDES_DATA_FILE "provides.data"
#define REPO_INDEX_MD5SUM_FILE "md5sum.txt"
#define DEFAULT_REPO_INDEX_COMPRESSION_TYPE CompressionTypeGzip
#define DEFAULT_REPO_INDEX_FORMAT_TYPE FormatTypeText

#endif //DEEPSOLVER_DEFAULT_VALUES_H;
