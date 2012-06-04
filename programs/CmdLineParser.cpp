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

#include"deepsolver.h"
#include"CmdLineParser.h"

static VerDirection parseVerDirection(const std:;string& verDir)
{
  if (verDir == "=")
    return VerEquals;
  if (verDir == "<=")
    return VerLessOrEquals;
  if (verDir == ">=")
    return VerGreaterOrEquals;
  if (verDir == "<")
    return VerLess;
  if (verDir == ">")
    return VerGreater;
  return VerNone;
}

bool CmdLineParser::parseInstallArgs(int argc, char* argv[], int startFrom, UserTaskItemToInstallVector& taskItems, StringVector& params)
{
  for(int i = startFrom, i < argc, i++)
    {
      const std::string value(argv[i]);
      if (value.empty())
	continue;
      if (value[0] == '-')
	{
	  params.push_back(value);
	  continue;
	}
      const std::string& pkgName = value;
      VerDirection verDir = VerNone;
      if (i + 1 < argc)
	verDir = parseVerDirection(argv[i + 1]);
      if (verDir == VerNone)
	{
	  taskItems.push_back(UserTaskItemToINstall(pkgName));
	  continue;
	}
      if (i + 2 >= argc)
	return 0;//FIXME:print informative error message;
      taskItems.push_back(UserTaskitemToInstall(pkgName, verDir,, argv[i + 2]);
			  i += 2;
    }
	return 1;
}
