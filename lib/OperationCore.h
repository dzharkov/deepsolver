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

/**\brief The abstract interface for continuous process interruption
 *
 * Various continuous processes (such as downloading) ask external object
 * to be sure the user do not want to interrupt the task being
 * performed. This class declares the interface for objects to provide
 * such information. It is called multiple times during the work. Any
 * negative answer causes immediate process cancelling.
 *
 * \sa OperationCore
 */
class AbstractOperationContinueRequest
{
public:
  /**\brief The destructor*/
  virtual ~AbstractOperationContinueRequest() {}

public:
  /**\brief Asks external structures to continue operation
   *
   * Implement this method to be able interrupt continuous operations.
   *
   * \return Non-zero means to continue operation
   */
  virtual bool onContinueOperationRequest() const = 0;
}; //class AbstractOperationContinueRequest; 

class AbstractIndexFetchListener
{
public:
  virtual ~AbstractIndexFetchListener() {}

public:
  virtual void onInfoFilesFetch() = 0;
  virtual void onIndexFetchBegin() = 0;
  virtual void onIndexFilesReading() = 0;
  virtual void onIndexFetchComplete() = 0;
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

  void doInstallRemove(const UserTask& task);

private:
  const ConfigCenter& m_conf;
}; //class OperationCore;

#endif //DEEPSOLVER_OPERATION_CORE_H;
