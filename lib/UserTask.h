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
    : verDir(VerNone) {}

 UserTaskItemToInstall(const std::string& n)
    : pkgName(n), verDir(VerNone) {}

  UserTaskItemToInstall(const std::string& n, VerDirection d, const std::string& v)
    : pkgName(n), verDir(d), version(v) {}

  virtual ~UserTaskItemToInstall() {}

public:
  std::string makeStr() const
  {
    return "#Needs to implement!#";//FIXME:
  }

public:
  std::string pkgName;
  VerDirection verDir;
  std::string version;
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
