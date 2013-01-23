/*
   Copyright 2011-2013 ALT Linux
   Copyright 2011-2013 Michael Pozhidaev

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

#include"os/system.h"
#include"logging.h"

#define DEEPSOLVER_LOGGER "deepsolver"

static int configLogLevel=LOG_INFO;
static bool configToConsole = 0;

static void logLine(int level, const char* line)
{
  assert(line);
  if (level > configLogLevel)
    return;
  //FIXME:syslog(level, "%s", line);
  if (!configToConsole)
    return;
  if (level <= LOG_CRIT)
    std::cerr << "FATAL:" << line << std::endl; else
    if (level <= LOG_ERR)
      std::cerr << "ERROR:" << line << std::endl; else
      if (level <= LOG_WARNING)
	std::cerr << "WARNING:" << line << std::endl; else
	std::cout << line << std::endl;
}

void logMsg(int level, const char* format, ...)
{
  if (!format)
    return;
  va_list args;
  va_start(args, format);
  char buf[4096];
  vsnprintf(buf, sizeof(buf), format, args);
  buf[sizeof(buf)-1]='\0';
  va_end(args);
  std::string str(buf);
  //FIXME:  removeNewLineChars(str);
  logLine(level, str.c_str());
}

void initLogging(int logLevel, bool toConsole)
{
  configLogLevel = logLevel;
  configToConsole = toConsole;
  //FIXME:    openlog(DEEPSOLVER_LOGGER, LOG_PID, LOG_DAEMON);
}
