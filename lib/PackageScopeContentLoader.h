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

class PackageScopeContentLoader: public PackageScopeContent
{
public:
  /**\brief The default constructor*/
  PackageScopeContentLoader() 
    : m_stringBuf(NULL) {}

  /**\brief The destructor*/
  virtual ~PackageScopeContentLoader() 
  {
    if (m_stringBuf != NULL)
      delete[] m_stringBuf;
  }

public:
  void loadFromFile(const std::string& fileName);

private:
  void readNames(std::ifstream& s, size_t namesBufSize);

private:
  char* m_stringBuf;
}; //class PackageScopeContentLoader;

#endif //DEEPSOLVER_PACKAGE_SCOPE_CONTENT_LOADER_H;
