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

#include"DeepsolverException.h"
#include"ConfigFile.h"
#include"ConfigData.h"

enum {
  ConfigErrorUnknownParam = 0,
  ConfigErrorIncompletePath = 1,
  ConfigErrorValueCannotBeEmpty = 2,
  ConfigErrorAddingNotPermitted = 3 
};

/**\brief Indicates the error in configuration data
 *
 * This class instance indicates any problem in Deepsolver configuration
 * structures. Be careful, it must not be confused with
 * ConfigFileException used to notify about configuration file syntax
 * error.
 *
 * The objects of ConfigException save the error code, optional string
 * argument which meaning depends on error code and error location in
 * configuration files if there is any. Some kind of problems are actual
 * without any reference to configuration file. For example, if some
 * parameter is not set at all but its value is required.
 *
 * \sa ConfigFileException ConfigCenter
 */
class ConfigException: public DeepsolverException
{
public:
  /**\brief The constructor
   *
   * \param [in] code The error code
   * \param [in] arg Optional string error argument with meaning depending on error code
   * \param [in] pos The information about configuration file line the error is associated
   */
  ConfigException(int code, const std::string& arg, const ConfigFilePosInfo& pos)
    : m_code(code),
      m_arg(arg),
      m_fileName(pos.fileName),
      m_lineNumber(pos.lineNumber),
      m_line(pos.line) {}

  /**\brief The constructor with no configuration file line info
   *
   * \param [in] code The error code
   * \param [in] arg Optional argument with meaning depending on error code
   */
  ConfigException(int code, const std::string& arg)
    : m_code(code),
      m_arg(arg),
      m_lineNumber(0) {}

public:
  /**\brief Returns the error code
   *
   * Use this method to get the error code
   *
   * \return The error code
   */
  int getCode() const
  {
    return m_code;
  }

  /**\brief Returns the optional string argument of the error
   *
   * Use this method to get string argument of the error. The meaning of
   * this argument depends on error code and can be empty.
   *
   * \return The optional string argument of the error
   */
  std::string getArg() const
  {
    return m_arg;
  }

  /**\brief Returns the name of the file associated with error position
   *
   * Use this method to get name of the file with invalid line. string
   * returned by this method can be empty because not all of the possible
   * errors have associated place in configuration files. Before using the
   * value returned by this method check the line number is greater than
   * zero since in case of error without associated place the line number
   * is always zero.
   *
   * \return The file name with the invalid line
   */
  std::string getFileName() const
  {
    return m_fileName;
  }

  /**\brief Returns number of the invalid line
   *
   * Use this method to get 1-based number of the line caused the
   * configuration problem. If this method returns zero it means there is
   * no line associated with the problem.
   *
   * \return The number of the line caused the problem or zero if there is no any
   */
  size_t getLineNumber() const
  {
    return m_lineNumber;
  }

  /**\brief Returns the line caused the configuration problem
   *
   * Use this method to get content of the line caused the configuration problem.
   *
   * \return The line caused configuration problem
   */
  std::string getLine() const
  {
    return m_line;
  }

  /**\brief Returns the string type of exception
   *
   * This method always returns "configuration" string.
   *
   * \return The type of exception (always "configuration")
   */
  std::string getType() const
  {
    return "configuration";
  }

  /**\brief Returns the single-line configuration error description
   *
   * Using of these method is recommended only for debug purposes.
   *
   * \return The single-line error description
   */
  std::string getMessage() const
  {
    return "FIXME";
  }

private:
  const int m_code;
  const std::string m_arg;
  const std::string m_fileName;
  const size_t m_lineNumber;
  const std::string m_line;
}; //class DeepsolverException;

class ConfigCenter: private AbstractConfigFileHandler
{
public:
  ConfigCenter() {}
  virtual ~ConfigCenter() {}

public:
  void loadFromFile(const std::string& fileName);
  void commit();

  const ConfRoot& root() const
  {
    return m_root;
  }

private:
    void onCoreConfigValue(const StringVector&path,
			   const std::string& sectArg,
			   const std::string& value,
			   bool adding, 
			   const ConfigFilePosInfo& pos);

    void onCoreDirConfigValue(const StringVector&path,
			   const std::string& sectArg,
			   const std::string& value,
			   bool adding, 
			   const ConfigFilePosInfo& pos);

    void onRepoConfigValue(const StringVector&path,
			   const std::string& sectArg,
			   const std::string& value,
			   bool adding, 
			   const ConfigFilePosInfo& pos);

  ConfRepo& findRepo(const std::string& name);

private://AbstractConfigFileHandler;
  void onConfigFileValue(const StringVector& path, 
			 const std::string& sectArg,
			 const std::string& value,
			 bool adding,
			 const ConfigFilePosInfo& pos);

private:
  ConfRoot m_root;
}; //class ConfigCenter;

#endif //DEEPSOLVER_CONFIG_CENTER_H;
