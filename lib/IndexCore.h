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

#ifndef DEEPSOLVER_INDEX_CORE_H
#define DEEPSOLVER_INDEX_CORE_H

#include"repo/RepoParams.h"

class IndexCoreException: public DeepsolverException 
{
public:
  IndexCoreException() {}
  IndexCoreException(const std::string& message):
    m_message(message) {}

  virtual ~IndexCoreException() {}

public:
  std::string getType() const
  {
    return "index core";
  }

  std::string getMessage() const
  {
    return m_message;
  }

private:
  const std::string m_message;
}; //class IndexCoreException;

class AbstractIndexConstructionListener
{
}; //class AbstractIndexConstructionListener;

class IndexCore
{
public:
  /**\brief The constructor*/
  IndexCore(AbstractIndexConstructionListener& listener) 
    : m_listener(listener) {}

  /**\brief The destructor*/
  virtual ~IndexCore() {}

public:
  void buildIndex(const RepoParams& params);
  void rebuildIndex(const RepoParams& params, const StringVector& toAdd, const StringVector& toRemove);

private:
  void collectRefs(const std::string& dirName, StringSet& res);

private:
  static std::string compressionExtension(char compressionType);
  static bool fileFromDirs(const std::string& fileName, const StringList& dirs);

private:
  AbstractIndexConstructionListener& m_listener;
}; //class IndexCore;

#endif //DEEPSOLVER_INDEX_CORE_H;
