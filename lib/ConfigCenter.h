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

#ifndef DEEPSOLVER_CONFIG_CENTER_H
#define DEEPSOLVER_CONFIG_CENTER_H

#include"ConfigFile.h"
#include"AttachedRepo.h"

struct ConfRoot
{
  std::string indexDir;
  std::string indexFetchDir;
  AttachedRepoVector repos;
}; //struct ConfRoot;

class ConfigCenter: private AbstractConfigFileHandler
{
public:
  ConfigCenter() {}
  virtual ~ConfigCenter() {}

public:
  void load(const std::string& configDir);

  const ConfRoot& root() const
  {
    return m_root;
  }

private://AbstractConfigFileHandler;
  void onConfigFileValue(const StringVector& path, 
			 const std::string& sectArg,
			 const std::string& value,
			 bool adding);

private:
  ConfRoot m_root;
}; //class ConfigCenter;

#endif //DEEPSOLVER_CONFIG_CENTER_H;
