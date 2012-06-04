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

#ifndef DEEPSOLVER_CMD_LINE_PARSER_H
#define DEEPSOLVER_CMD_LINE_PARSER_H

#include"UserTask.h"

class CmdLineParser
{
public:
  CmdLineParser() {}
  virtual ~CmdLineParser() {}

public:
  bool parseInstallArgs(int argc, char* argv[], int startFrom, UserTaskItemToInstallVector& taskItems, StringVector& params);
}; //class CmdLineParser;

#endif //DEEPSOLVER_CMD_LINE_PARSER_H;
