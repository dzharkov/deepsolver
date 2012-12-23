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

#include"ConfigFile.h"
#include"ConfigData.h"

/**\brief The central configuration processing class
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

  void printConfigData(std::ostream& s) const;

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
    ValueTypeBoolean = 3,
ValueTypeUint = 4
  };

class Value
{
public:
  public:
  Value() {}
  virtual ~Value() {}

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

    std::string pathToString() const
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
}; //class Value;

  class StringValue: public Value 
  {
  public:
  StringValue()
    : canBeEmpty(0),
      value(NULL) {}

  StringValue(std::string& v)
    : canBeEmpty(0),
      value(&v) {}

    virtual ~StringValue() {}

  public:
    bool canBeEmpty;
    std::string* value;
  }; //class StringValue;

  class StringListValue : public Value
  {
  public:
  StringListValue()
    : canContainEmptyItem(0),
      canBeEmpty(0),
      value(NULL) {}

  StringListValue(StringVector& v)
    : canContainEmptyItem(0),
      canBeEmpty(0),
      value(&v) {}

    virtual ~StringListValue() {}

  public:
    bool canContainEmptyItem;
    bool canBeEmpty;
    std::string delimiters;
    StringVector* value;
  }; //class StringListValue;

  class BooleanValue: public Value
  {
  public:
  BooleanValue()
    : value(NULL) {}

  BooleanValue(bool& v)
    : value(&v) {}

    virtual ~BooleanValue() {}

  public:
    bool* value;
  }; //class BooleanValue;

  class UintValue: public Value
  {
  public:
  UintValue()
    : value(NULL) {}

  UintValue(unsigned int& v)
    : value(&v) {}

    virtual ~UintValue() {}

  public:
    unsigned int* value;
  }; //class UintValue;

private:
  void initValues();
  void initRepoValues();
  void initProvideValues();
  int getParamType(const StringVector& path, const std::string& sectArg, const ConfigFilePosInfo& pos) const;

private:
  void addStringParam3(const std::string& path1,
		       const std::string& path2,
		       const std::string& path3,
		       std::string& value);

  void addNonEmptyStringParam3(const std::string& path1,
			       const std::string& path2,
			       const std::string& path3,
			       std::string& value);

  void addStringListParam3(const std::string& path1,
		       const std::string& path2,
		       const std::string& path3,
		       StringVector& value);

  void addNonEmptyStringListParam3(const std::string& path1,
			       const std::string& path2,
			       const std::string& path3,
			       StringVector& value);

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
		       StringListValue& stringListValue);

  void processBooleanValue(const StringVector& path, 
			 const std::string& sectArg,
			 const std::string& value,
			 bool adding,
			 const ConfigFilePosInfo& pos);

  void findBooleanValue(const StringVector& path, 
			 const std::string& sectArg,
		       BooleanValue& booleanValue);

  void processUintValue(const StringVector& path, 
			 const std::string& sectArg,
			 const std::string& value,
			 bool adding,
			 const ConfigFilePosInfo& pos);

  void findUintValue(const StringVector& path, 
			 const std::string& sectArg,
		       UintValue& uintValue);

private://AbstractConfigFileHandler;
  void onConfigFileValue(const StringVector& path, 
			 const std::string& sectArg,
			 const std::string& value,
			 bool adding,
			 const ConfigFilePosInfo& pos);

private:
  typedef std::vector<StringValue> StringValueVector;
  typedef std::vector<StringListValue> StringListValueVector;
  typedef std::vector<BooleanValue> BooleanValueVector;
  typedef std::vector<UintValue> UintValueVector;

private:
  ConfRoot m_root;
  StringValueVector m_stringValues;
  StringListValueVector m_stringListValues;
  BooleanValueVector m_booleanValues;
  UintValueVector m_uintValues;
}; //class ConfigCenter;

#endif //DEEPSOLVER_CONFIG_CENTER_H;
