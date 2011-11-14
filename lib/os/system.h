
#ifndef FIXME_SYSTEM_H
#define FIXME_SYSTEM_H

#include<stdlib.h>
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
#include<sys/socket.h>
#include<arpa/inet.h>
#include<resolv.h>
#include<sys/un.h>
#include<pthread.h>
#include<fcntl.h>
#include<iconv.h>
#include<locale.h>
#include<time.h>


typedef std::vector<size_t> SizeVector;
typedef std::vector<std::string> StringVector;
typedef std::list<std::string> StringList;
typedef std::map<std::string, std::string> StringToStringMap;

#define ERRNO_MSG (strerror(errno))
#define NEW_FILE_MODE (S_IRWXU | S_IRWXG | S_IRWXO)

#endif //FIXME_SYSTEM_H;
