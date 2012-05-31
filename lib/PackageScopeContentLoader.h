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

#ifndef DEEPSOLVER_PACKAGE_SCOPE_CONTENT_LOADER_H
#define DEEPSOLVER_PACKAGE_SCOPE_CONTENT_LOADER_H

#include"PackageScopeContent.h"

class PackageScopeContentLoader
{
public:
  /**\brief The constructor*/
  PackageScopeContentLoader(PackageScopeContent& content) 
    : m_c(content) {}

  /**\brief The destructor*/
  virtual ~PackageScopeContentLoader() {}

public:
  void loadFromFile(const std::string& fileName);

private:
  void readNames(std::ifstream& s, size_t namesBufSize);

private:
  PackageScopeContent& m_c;
}; //class PackageScopeContentLoader;

#endif //DEEPSOLVER_PACKAGE_SCOPE_CONTENT_LOADER_H;
