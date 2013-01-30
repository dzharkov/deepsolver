/*
   Copyright 2011-2013 ALT Linux
   Copyright 2011-2013 Michael Pozhidaev

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
 * to be sure a user do not want to interrupt the task being
 * performed. This class declares the interface for objects to provide
 * such information. It is called multiple times during the work. Any
 * negative answer causes immediate process cancelling.
 *
 * \sa OperationCore
 */
class AbstractOperationContinueRequest
{
public:
  /**\brief The default constructor*/
  AbstractOperationContinueRequest() {}

  /**\brief The destructor*/
  virtual ~AbstractOperationContinueRequest() {}

public:
  /**\brief Confirms a process should be continued or signals it must be cancelled
   *
   * Implement this method for proper interruption of continuous tasks.
   *
   * \return Non-zero means to continue operation
   */
  virtual bool onContinueOperationRequest() const = 0;
}; //class AbstractOperationContinueRequest; 

/**\brief The abstract interface to listen progress of index fetching
 *
 * This abstract interface receives complete information about index
 * fetching process. This information may be used to let user know what
 * is happening in arbitrary time moment.
 *
 * \sa OperationCore
 */
class AbstractIndexFetchListener
{
public:
  /**\brief The default constructor*/
AbstractIndexFetchListener() {}

  /**\brief The destructor*/
  virtual ~AbstractIndexFetchListener() {}

public:
  /**\brief Notifies basic repository headers downloading is in progress
   *
   * Implement this method to show notification basic headers come to machine.
   */
  virtual void onInfoFilesFetch() = 0;

  /**\brief Notifies index fetching task is initiated
   *
   * Implement this method to catch a moment when fetching process begins.
   */
  virtual void onIndexFetchBegin() = 0;

  /**\brief Notifies files are fetched and are being read
   *
   * Implement this method to know fetched files reading begins.
   */
  virtual void onIndexFilesReading() = 0;

  /**\brief Notifies index fetching process is finished
   *
   * Implement this method to know everything is done.
   */
  virtual void onIndexFetchComplete() = 0;

  /**\brief Notifies about a progress of each file fetching
   *
   * This method is called each time new portion of data is obtained and let 
   * you know how many total percents of work are completed.
   *
   * \param [in] currentPartPercents How many percents of current part are done
   * \param [in] totalPercents How many total percents are done
   * \param [in] partNumber A number of current part
   * \param [in] partCount Total number of parts
   * \param [in] currentPartSize A size of current part in bytes
   * \param [in] currentPartName Name of current part (usually URL)
   */
  virtual void onIndexFetchStatus(unsigned char currentPartPercents,
				  unsigned char totalPercents,
				  size_t partNumber,
				  size_t partCount,
				  size_t currentPartSize,
				  const std::string& currentPartName) = 0;
}; //class AbstractIndexFetchListener;

class AbstractTransactionListener
{
public:
  /**\brief The default constructor*/
  AbstractTransactionListener() {}

  /**\brief The destructor*/
  virtual ~AbstractTransactionListener() {}

public:
  virtual void onAvailablePkgListProcessing() = 0;
  virtual void onInstalledPkgListProcessing() = 0;
  virtual void onInstallRemovePkgListProcessing() = 0;
}; //class abstractTransactionListener;

/**\brief The main class for package managing
 *
 * The OperationCore class is the central class of Deepsolver project. It
 * allows every client to perform various manipulations with packages
 * including installation, removing and upgrading. In addition this class
 * takes care of information gathered about attached repositories. 
 *
 * Be careful, each method of the OperationCore class throws a number of
 * exceptions. Their exact set depends on method purpose. The main
 * structure used for configuring is a ConfigCenter class instance
 * provided by a reference to OperationCore constructor.
 *
 * According to accepted project design there are two additional classes
 * purposed for direct client using as well as OperationCore itself:
 * IndexCore and InfoCore. The former takes care about index construction
 * and the second provides various information about known
 * packages. These three classes are the main classes end-user could be
 * interested in.
 *
 * \sa OperationException
 * \sa IndexCore InfoCore
 */
class OperationCore
{
public:
  /**\brief The constructor
   *
   * \param [in] conf A reference to configuration data object
   */
  OperationCore(const ConfigCenter& conf): 
    m_conf(conf)  {}

  /**\brief The destructor*/
    virtual ~OperationCore() {}

public:
    /**\brief Updates repository content information
     *
     * \throws OperationException
     * \throws CurlException
     * \throws SystemException
     */
  void fetchIndices(AbstractIndexFetchListener& listener,
		    const AbstractOperationContinueRequest& continueRequest) const;

  void transaction(AbstractTransactionListener& listener, const UserTask& task) const;
  std::string generateSat(AbstractTransactionListener& listener, const UserTask& task) const;
  void printPackagesByRequire(const NamedPkgRel& rel, std::ostream& s) const;


private:
  const ConfigCenter& m_conf;
}; //class OperationCore;

#endif //DEEPSOLVER_OPERATION_CORE_H;
