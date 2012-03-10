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

#ifndef DEEPSOLVER_OPERATION_CORE_H
#define DEEPSOLVER_OPERATION_CORE_H

#include"ConfigCenter.h"
#include"AttachedRepo.h"

class OperationCore
{
public:
  OperationCore(const ConfigCenter& conf): 
    m_conf(conf)  {}
  virtual ~OperationCore(); 

public:
  void load();

private:
  void destroyRepos();
  void fillRepos();

private:
  const ConfigCenter& m_conf;
  AttachedRepoVector m_repos;
}; //class OperationCore;

#endif //DEEPSOLVER_OPERATION_CORE_H;
