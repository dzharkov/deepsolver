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

enum {
  IndexErrorInternalIOProblem = 0
};

class IndexCoreException: public DeepsolverException 
{
public:
  IndexCoreException(int code)
    : m_code(code) {}

  virtual ~IndexCoreException() {}

public:
  std::string getType() const
  {
    return "index core";
  }

  std::string getMessage() const
  {
    return "FIXME";
  }

private:
  const int m_code;
}; //class IndexCoreException;

class AbstractIndexConstructionListener
{
public:
  virtual ~AbstractIndexConstructionListener() {}

public:
  virtual void onReferenceCollecting(const std::string& path) = 0;
  virtual void onPackageCollecting(const std::string& path) = 0;
  virtual void onProvidesCleaning() = 0;
  virtual void onPatchingFile(const std::string& fileName) = 0;
  virtual void onNoTwiceAdding(const std::string& fileName) = 0;
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
  void fixReferences(const RepoParams& params);

private:
  void collectRefs(const std::string& dirName, StringSet& res);

private:
  static std::string compressionExtension(char compressionType);
  static bool fileFromDirs(const std::string& fileName, const StringVector& dirs);

private:
  AbstractIndexConstructionListener& m_listener;
}; //class IndexCore;

#endif //DEEPSOLVER_INDEX_CORE_H;
