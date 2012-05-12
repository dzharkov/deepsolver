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

#include"DeepsolverException.h"
#include"ConfigCenter.h"

enum {
  OperationErrorInvalidInfoFile = 0,
  OperationErrorInvalidChecksum = 1
};

/**\brief The exception class for general operation problem reporting
 *
 * This class is purposed for various general operation problem
 * reporting. The errors can be thrown only by the methods of
 * OperationCore class covering transaction processing as well as index
 * updating and information retrieving. General error types are checksum
 * mismatch, invalid content of repo file and so on. Downloading problems
 * have their own exception class called CurlException.
 *
 * \sa OperationCore CurlException System Exception RpmException
 */
class OperationException
{
public:
  /**\brief The constructor
   *
   * \param [in] code The error code
   */
  OperationException(int code) 
    : m_code(code) {}

  virtual ~OperationException() {}

public:
  /**\brief Returns the error code
   *
   * Use this method to get error code.
   *
   * \return The error code
   */
  int getCode() const
  {
    return m_code;
  }

  /**\brief Returns the type of exception
   *
   * Use this method to get type of exception (always "operation" by this class). 
   *
   * \return The type of exception (always "operation")
   */
  std::string getType() cost
  {
    return "operation";
  }

  std::string getMessage() const
  {
    switch (m_code)
      {
      case OperationErrorInvalidInfoFile:
	return "repository info file has an invalid content";
	break;
      case OperationErrorInvalidChecksum :
	return "one or more files were corrupted (invalid checksum)";
	break;
      default:
	assert(0);
      } //switch(m_code);
    return "";//just to reduce warning messages;
  }

private:
  const int m_code;
}; //class OperationException;

class AbstractOperationContinueRequest
{
public:
  virtual ~AbstractOperationContinueRequest() {}

public:
  virtual bool onContinueOperationRequest() const = 0;
}; //class AbstractOperationContinueRequest; 

class AbstractIndexFetchListener
{
public:
  virtual ~AbstractIndexFetchListener() {}

public:
  virtual void onIndexFetchStatus(unsigned char currentPartPercents,
				  unsigned char totalPercents,
				  size_t partNumber,
				  size_t partCount,
				  size_t currentPartSize,
				  const std::string& currentPartName) = 0;
}; //class AbstractIndexFetchListener;

class OperationCore
{
public:
  OperationCore(const ConfigCenter& conf): 
    m_conf(conf)  {}

    virtual ~OperationCore() {}

public:
  void fetchIndices(AbstractIndexFetchListener& listener,
		    const AbstractOperationContinueRequest& continueRequest);

private:
  const ConfigCenter& m_conf;
}; //class OperationCore;

#endif //DEEPSOLVER_OPERATION_CORE_H;
