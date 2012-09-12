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

#ifndef DEEPSOLVER_LOGGING_H
#define DEEPSOLVER_LOGGING_H

void initLogging(int logLevel, bool toConsole);

/**\brief Processes log message
 *
 * This is the main function to make single log message. The message text
 * can be specified as printf() formatted string with consequent
 * parameters. Log level value can be provided as syslog level constants.
 *
 * \param [in] level The error level for this message
 * \param [in] format The message text
 */
void logMsg(int level, const char* format,... );

#endif //DEEPSOLVER_LOGGING_H;
