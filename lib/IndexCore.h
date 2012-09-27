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

/**\brief Indicates repository index manipulation problem
 *
 * The instance of this exception is thrown on some index creation or
 * patching problems such as corrupted file, not empty target directory
 * etc. This exception created in addition to general exception classes
 * like SystemException, RpmException and so on which can be also thrown
 * during index operations.
 *
 * \sa IndexCore SystemException RpmException Md5FileException
 */
class IndexCoreException: public DeepsolverException 
{
public:
  enum {
    InternalIOProblem = 0,
    DirectoryNotEmpty = 1,
    CorruptedFile = 2,
MissedChecksumFileName = 3
  };

public:
  /**\brief The constructor with error code specification
   *
   * \param [in] code The error code of error occurred
   */
  IndexCoreException(int code)
    : m_code(code) {}

  /**\brief The constructor with error code and string argument specification
   *
   * \param [in] code The error code of problem occurred
   * \param [in] arg The string argument for problem type
   */
  IndexCoreException(int code, const std::string& arg)
    : m_code(code), m_arg(arg) {}

  /**\brief The destructor*/
  virtual ~IndexCoreException() {}

public:
  /**\brief Returns the error code
   *
   * Use this method to get error code value.
   *
   * \return The error code value
   */
  int getCode() const
  {
    return m_code;
  }

  /**\brief Returns the additional string argument
   *
   * Use this method to get additional string argument of the problem occurred.
   *
   * \return The additional string argument
   */
  const std::string& getArg() const
  {
    return m_arg;
  }

  std::string getType() const
  {
    return "index core";
  }

  std::string getMessage() const
  {
    switch(m_code)
      {
      case InternalIOProblem:
	return "Internal I/O problem";
      case DirectoryNotEmpty:
	return "Directory \'" + m_arg + "\' is not empty";
      case CorruptedFile:
	return "File \'" + m_arg + "\' is corrupted (wrong checksum)";
      case MissedChecksumFileName:
	return "No checksum file name in repository parameters";
      default:
	assert(0);
      } //switch(m_code);
    return "";
  }

private:
  const int m_code;
  const std::string m_arg;
}; //class IndexCoreException;

class AbstractIndexConstructionListener
{
public:
  virtual ~AbstractIndexConstructionListener() {}

public:
  virtual void onReferenceCollecting(const std::string& path) = 0;
  virtual void onPackageCollecting(const std::string& path) = 0;
  virtual void onProvidesCleaning() = 0;
  virtual void onChecksumWriting() = 0;
  virtual void onChecksumVerifying() = 0;
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
  void refilterProvides(const RepoParams& params);

private:
  void collectRefs(const std::string& dirName, StringSet& res);

private:
  AbstractIndexConstructionListener& m_listener;
}; //class IndexCore;

#endif //DEEPSOLVER_INDEX_CORE_H;
