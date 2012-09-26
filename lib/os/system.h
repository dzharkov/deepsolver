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

#ifndef DEEPSOLVER_SYSTEM_H
#define DEEPSOLVER_SYSTEM_H

#include<stdlib.h>
#include<stdint.h>
#include<stdarg.h>
#include<stdio.h>
#include<string.h>
#include<math.h>

#include<string>
#include<vector>
#include<list>
#include<set>
#include<map>
#include<sstream>
#include<fstream>
#include<iostream>
#include<algorithm>
#include<memory>

#include<sys/types.h>
#include<unistd.h>
#include<syslog.h>
#include<dirent.h>
#include<errno.h>
#include<assert.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<signal.h>
#include<regex.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<resolv.h>
#include<sys/un.h>
#include<pthread.h>
#include<fcntl.h>
#include<iconv.h>
#include<locale.h>
#include<time.h>

typedef std::vector<bool> BoolVector;
typedef std::vector<size_t> SizeVector;
typedef std::vector<const char*> ConstCharStarVector;
typedef std::vector<std::string> StringVector;
typedef std::list<std::string> StringList;
typedef std::set<std::string> StringSet;
typedef std::map<std::string, std::string> StringToStringMap;

#define ERRNO_MSG (strerror(errno))
#define NEW_FILE_MODE (S_IRWXU | S_IRWXG | S_IRWXO)

#endif //DEEPSOLVER_SYSTEM_H;
