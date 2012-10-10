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
 * errors.
 *
 * The objects of ConfigException provide the error code, optional string
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

/**\brief Central configuration processing class
 *
 * This class designed as central place to store all configuration
 * information necessary for all operations except repository index
 * building. Every client application should create the instance of this
 * class, process with it configuration files it need and provide the
 * reference to OperationCore or InfoCore classes depending on required
 * task.
 *
 * Configuration center class strongly associated with ConfigFile class
 * since it is used for configuration file syntax processing. During
 * configuration file reading two types of exceptions can be thrown:
 * ConfigFileException and ConfigException. The first type is used for
 * syntax error indication only, the second one is designed for general
 * configuration data validation problems.
 *
 * Do not forget to call commit() method after configuration files
 * reading. Some data may be left unprepared without commit() method
 * invocation. After configuration files processing configuration data is
 * placed in various structures gathered in ConfRoot type and can be
 * accessed directly.
 *
 * \sa ConfRoot ConfigFile ConfigException ConfigFileException
 */
class ConfigCenter: private AbstractConfigFileHandler
{
public:
  /**\brief The default constructor*/
  ConfigCenter() {initValues();}

  /**\brief The destructor*/
  virtual ~ConfigCenter() {}

public:
  /**\brief reads one configuration file
   *
   * This method reads one configuration file parses it and saves processed
   * values into internal structures for further access. You can call it
   * multiple times for different files but do not forget make final
   * invocation of commit() method to validate received data.
   *
   * This method throws two types of exceptions: ConfigFileException to
   * indicate syntax problems and ConfigException for general errors.
   *
   * \param [in] fileName Name of the file to read configuration data from
   *
   * \sa ConfigException ConfigFileException
   */
  void loadFromFile(const std::string& fileName);

  /**\brief Verifies read configuration data
   *
   * This method makes final configuration data preparing and performs
   * various checks to be sure the data is valid. In case of errors
   * ConfigException or ConfigFileException can be thrown. This method call
   * is strongly required for proper further execution.
   *
   * \sa ConfigException ConfigFileException
   */
  void commit();

  /**\brief Returns the reference to parsed configuration data
   *
   * This method used for access to parsed values after configuration data reading during operations processing.
   *
   * \return The reference to parsed data
   *
   * \sa ConfRoot
   */
  const ConfRoot& root() const
  {
    return m_root;
  }

private:
  enum {
    ValueTypeString = 1,
    ValueTypeStringList = 2,
    ValueTypeBoolean = 3
  };

  class StringValue
  {
  public:
  StringValue()
    : canBeEmpty(0),
      value(NULL) {}

  StringValue(std::string& v)
    : canBeEmpty(0),
      value(&v) {}

  public:
    bool pathMatches(const StringVector& p, const std::string& a) const 
      {
	if (path.size() != p.size())
	  return 0;
	for(StringVector::size_type i = 0;i < path.size();i++)
	  if (path[i] != p[i])
	    return 0;
	if (!sectArg.empty() && sectArg != a)
	  return 0;
	return 1;
      }

  public:
    std::string pathToString()
    {
      assert(!path.empty());
      std::string value = path[0];
      if (!sectArg.empty())
	value += " \"" + sectArg + "\"";
      for(StringVector::size_type i = 1;i < path.size();i++)
	value += "." + path[i];
      return value;
    }

  public:
    StringVector path;
    std::string sectArg;
    bool canBeEmpty;
    std::string* value;
  }; //class StringValue;

  class StringListValue
  {
  public:
  StringListValue()
    : canCotainEmptyItem(0),
      value(NULL) {}

  StringValue(std::string& v)
    : canContainEmptyItem(0),
      value(&v) {}

  public:
    bool pathMatches(const StringVector& p, const std::string& a) const 
      {
	if (path.size() != p.size())
	  return 0;
	for(StringVector::size_type i = 0;i < path.size();i++)
	  if (path[i] != p[i])
	    return 0;
	if (!sectArg.empty() && sectArg != a)
	  return 0;
	return 1;
      }

  public:
    std::string pathToString()
    {
      assert(!path.empty());
      std::string value = path[0];
      if (!sectArg.empty())
	value += " \"" + sectArg + "\"";
      for(StringVector::size_type i = 1;i < path.size();i++)
	value += "." + path[i];
      return value;
    }

  public:
    StringVector path;
    std::string sectArg;
    bool canContainEmptyItem;
    std::string delimiters;
    StringVector* value;
  }; //class StringListValue;

private:
  void initValues();
  void reinitRepoValues();
  int getParamType(const StringVector& path, const std::string& sectArg, const ConfigFilePosInfo& pos) const;

  void addStringParam3(const std::string& path1,
		       const std::string& path2,
		       const std::string& path3,
		       std::string& value);

  void addNonEmptyStringParam3(const std::string& path1,
			       const std::string& path2,
			       const std::string& path3,
			       std::string& value);

private:
  void processStringValue(const StringVector& path, 
			 const std::string& sectArg,
			 const std::string& value,
			 bool adding,
			 const ConfigFilePosInfo& pos);

  void findStringValue(const StringVector& path, 
			 const std::string& sectArg,
		       StringValue& stringValue);

  void processStringListValue(const StringVector& path, 
			 const std::string& sectArg,
			 const std::string& value,
			 bool adding,
			 const ConfigFilePosInfo& pos);

  void findStringListValue(const StringVector& path, 
			 const std::string& sectArg,
		       StringValue& stringValue);


private://AbstractConfigFileHandler;
  void onConfigFileValue(const StringVector& path, 
			 const std::string& sectArg,
			 const std::string& value,
			 bool adding,
			 const ConfigFilePosInfo& pos);

private:
  typedef std::vector<StringValue> StringValueVector;
  typedef std::vector<StringListValue> StringListValueVector;

private:
  ConfRoot m_root;
  StringValueVector m_stringValues, m_repoStringValues;
  StringListValueVector m_stringListValues, m_repoStringListValues;
}; //class ConfigCenter;

#endif //DEEPSOLVER_CONFIG_CENTER_H;
