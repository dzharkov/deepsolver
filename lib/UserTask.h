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

#ifndef DEEPSOLVER_USER_TASK_H
#define DEEPSOLVER_USER_TASK_H

class UserTaskItemToInstall
{
public:
  UserTaskItemToInstall()
    : less(0),
      equals(0),
      greater(0) {}

public:
  std::string makeStr() const
  {
    assert(!less || !greater);
    std::string s = pkgName;
    if (version.empty())
      return s;
    s += " ";
    assert(less || equals || greater);
    if (less && equals)
      s += "less than or equals"; else
    if (greater && equals)
      s += "greater than or equals"; else
    if (less)
      s += "less than"; else
    if (equals)
s += "equals"; else
    if (greater)
      s += "greater than";
    s += " " + version;
    return s;
    return "#Needs to implement!#";
  }

public:
  std::string pkgName;
  std::string version;
  bool less, equals, greater;
}; //class UserTaskItemToINstall;

typedef std::vector<UserTaskItemToInstall> UserTaskItemToInstallVector;
typedef std::list<UserTaskItemToInstall> UserTaskItemToInstallList;

class UserTask 
{
public:
  UserTask() {}
  ~UserTask() {}

public:
  UserTaskItemToInstallVector itemsToInstall;
  StringSet urlsToInstall;
  StringSet namesToRemove;
}; //class UserTask;

#endif //DEEPSOLVER_USER_TASK_H;
