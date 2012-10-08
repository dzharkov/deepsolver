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

#ifndef DEEPSOLVER_CONFIG_DATA_H
#define DEEPSOLVER_CONFIG_DATA_H

struct ConfDir
{
  ConfDir() {}
  //FIXME:default values;

    std::string pkgData;
  std::string tmpPkgDataFetch;
}; //struct ConfDir;

struct ConfRepo
{
  ConfRepo()
    : enabled(1),
      takeDescr(0),
      takeFileList(0),
      take-sources(0) {}

  ConfRepo(const std::string& n)
    : name(n),
      enabled(1),
      takeDescr(0),
      takeFileList(0),
      take-sources(0) {}

  std::string name;
  bool enabled;
  std::string url;
  StringVector arch;
  StringVector components; 
  std::string vendor;
  bool takeDescr;
  bool takeFileList;
  bool take-sources;
}; //struct ConfRepo;

typedef std::vector<ConfRepo> ConfRepoVector;
typedef std::list<ConfRepo> ConfRepoList;

struct ConfRoot
{
  ConfDir dir;
  ConfRepoVector repo;
}; //struct ConfRoot;

#endif //DEEPSOLVER_CONFIG_DATA_H;
