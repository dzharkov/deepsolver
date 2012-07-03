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

#ifndef DEEPSOLVER_CONFIG_FILE_H
#define DEEPSOLVER_CONFIG_FILE_H

#include"DeepsolverException.h"

enum {
  ConfigErrorSectionInvalidType = 0,
  ConfigErrorSectionWaitingOpenBracket = 1,
  ConfigErrorSectionWaitingName = 2,
  ConfigErrorSectionInvalidNameChar = 3,
  ConfigErrorSectionWaitingCloseBracketOrArg = 4,
  ConfigErrorSectionUnexpectedArgEnd = 5,
  ConfigErrorSectionWaitingCloseBracket = 6,
  ConfigErrorValueWaitingName = 7,
  ConfigErrorValueInvalidNameChar = 8,
  ConfigErrorValueWaitingAssignOrNewName = 9,
  ConfigErrorValueWaitingNewName = 10,
  ConfigErrorValueUnexpectedValueEnd = 11
};

/**\brief The exception class for config file syntax errors
 *
 * This class instance is thrown when configuration file syntax error was
 * encountered. The client application can access various information
 * with it about the problem such as file name, line number, character
 * position, line content and error code. Be careful, character position
 * is given in unibyte string representation and in case of using UTF-8
 * sequences it require additional processing. Note, that this class does
 * not provide any text descriptions. 
 *
 * \sa ConfigException AbstractConfigFileHandler ConfigFile
 */
class ConfigFileException: public DeepsolverException
{
public:
  /**\brief The constructor
   *
   * \param [in] code The error code describing the type of error
   * \param [in] fileName Name of a file being processed
   * \param [in] lineNumber The number of a line with encountered error
   * \param [in] pos The problem character position with unibyte coding
   * \param [in] line The text of an invalid line
   */
  ConfigFileException(int code,
		      const std::string& fileName,
		      size_t lineNumber,
		      std::string::size_type pos,
		      const std::string& line)
    : m_code(code),
      m_fileName(fileName),
      m_lineNumber(lineNumber),
      m_pos(pos),
      m_line(line) {}

  /**\brief The destructor*/
  virtual ~ConfigFileException() {}

public:
  /**\brief Returns the type of encountered error
   *
   * Use this method to get type of error.
   *
   * \return The encountered error code
   */
  int getCode() const
  {
    return m_code;
  }

  /**\brief Returns the name of a file with invalid line
   *
   * Use this method to get name of a file with an invalid line.
   *
   * \return Name of a file with an invalid line
   */
  const std::string& getFileName() const
  {
    return m_fileName;
  }

  /**\brief Returns number of an invalid line
   *
   * Use this method to get number of line with the syntax problem.
   *
   * \return Number of an invalid line
   */
  size_t getLineNumber() const
  {
    return m_lineNumber;
  }

  /**\brief Returns the invalid character position
   *
   * Use this method to get the invalid character position in unibyte character coding.
   *
   * \return The number of the invalid character
   */
  std::string::size_type getPos() const
  {
    return m_pos;
  }

  /**\brief Returns the text of the invalid line
   *
   * Use this method to get text of the invalid line.
   *
   * \return The content of the invalid line
   */
  const std::string& getLine() const
  {
    return m_line;
  }

  /**\brief Returns the error type
   *
   * This method always returns "config syntax" string.
   *
   * \return The type of the error (always "config syntax")
   */
  std::string getType() const
  {
    return "config syntax";
  }

  /**\brief Returns the single-line description of the error
   *
   * Use this method to get single-line description (recommended only for debug purposes).
   *
   * \return The single-line error description
   */
  std::string getMessage() const
  {
    std::string msg;
    switch(m_code)
      {
      case ConfigErrorSectionInvalidType:
	msg = "invalid section type";
	break;
      case ConfigErrorSectionWaitingOpenBracket:
	msg = "missed opening bracket";
	break;
      case ConfigErrorSectionWaitingName:
	msg = "missed section name";
	break;
      case ConfigErrorSectionInvalidNameChar:
	msg = "invalid section name character";
	break;
      case ConfigErrorSectionWaitingCloseBracketOrArg:
	msg = "missed closing bracket or section header argument";
	break;
      case ConfigErrorSectionUnexpectedArgEnd:
	msg = "unexpected argument end";
	break;
      case ConfigErrorSectionWaitingCloseBracket:
	msg = "missed closing bracket";
	break;
      case ConfigErrorValueWaitingName:
	msg = "missed parameter name";
	break;
      case ConfigErrorValueInvalidNameChar:
	msg = "invalid parameter name character";
	break;
      case ConfigErrorValueWaitingAssignOrNewName:
	msg = "illegal new name component or missed equals sign";
	break;
      case ConfigErrorValueWaitingNewName:
	msg = "illegal new parameter name component";
	break;
      case ConfigErrorValueUnexpectedValueEnd:
	msg = "unexpected value end";
	break;
      default:
	assert(0);
	return "";
      } //switch();
    std::ostringstream ss;
    ss << m_fileName << "(" << m_lineNumber << "):" << msg;
    return ss.str();
  }

private:
  const int m_code;
  const std::string m_fileName;
  const size_t m_lineNumber;
  const std::string::size_type m_pos;
  const std::string m_line;
}; //class ConfigFileException;

struct ConfigFilePosInfo
{
ConfigFilePosInfo(const std::string& f, size_t ln, const std::string& l)
    : fileName(f), lineNumber(ln), line(l) {}

  const std::string& fileName;
  size_t lineNumber;
  const std::string& line;
}; //struct ConfigFilePosInfo;

/**\brief The interface to listen parsed configuration file parameters
 *
 * This abstract class describes the interface to get every parsed
 * configuration line parameter. It is generally purposed for various
 * configuration file handlers. It does not take care about any
 * configuration semantics.
 *
 * The events are sent only for every parameter but not for section
 * headers. It is enough for current requirements but in future the
 * section events must present as well as parameter events now. In
 * additional parameter events must be slightly extended to provide
 * complete information necessary for constructing text form of
 * configuration file back. It makes possible creation of various
 * configuration data editing utilities.
 *
 * \sa ConfigFile ConfigFilePosInfo
 */
class AbstractConfigFileHandler
{
public:
  /**\brief The destructor*/
  virtual ~AbstractConfigFileHandler() {}

public:
  /**\brief The event about newly parsed configuration file parameter
   *
   *
   * This method transmits information about one parsed configuration line
   * parameter with complete information for its proper processing The
   * information includes parameter path, first section argument if it has
   * any boolean flag to indicate is it new assignment or adding to already
   * existing value, the parameter value and information about parameter
   * position in file.
   *
   * The parameter name consists of several components called parameter
   * path. The last path component is the exact parameter name and it is
   * always present, all previous components are the section names and can
   * be mentioned in file either in section header or in the line of
   * parameter. In both cases name components are delimited by dots. If
   * section argument is not empty it is always associated with first
   * component in parameter path.
   *
   * \param [in] path The parameter path including its exact name
   * \param [in] sectArg The argument for the first path component if it has any
   * \param [in] value The parameter value
   \param [in] * adding The flag for indication not to erase previously set value if was any and perform adding
   * \param [in] pos The information about the line position in the configuration file
   */
  virtual void onConfigFileValue(const StringVector& path, 
				 const std::string& sectArg,
				 const std::string& value,
				 bool adding,
				 const ConfigFilePosInfo& pos) = 0;
}; //class AbstractConfigHandler;

class ConfigFile
{
private:
  enum {
    ModeAssign = 0,
    ModeAdding = 1
  };

public:
  ConfigFile(AbstractConfigFileHandler& handler, const std::string fileName)
    : m_handler(handler),
      m_fileName(fileName),
      m_linesProcessed(0),
      m_sectLevel(0),
      m_sectArgPos(0),
      m_assignMode(ModeAssign) {}

  virtual ~ConfigFile() {}

public:
  void processLine(const std::string& line);

public:
  void processSection(const std::string& line);
  void processValue(const std::string& line);
  void stopSection(int state, std::string::size_type pos, const std::string& line);
  void stopParam(int state, std::string::size_type pos, const std::string& line);

private:
  AbstractConfigFileHandler& m_handler;
  const std::string m_fileName;
  size_t m_linesProcessed;
  StringVector m_path;
  StringVector::size_type m_sectLevel;
  std::string m_sectArg, m_paramValue;
  std::string::size_type m_sectArgPos;
  int m_assignMode;
}; //class ConfigFile;

#endif //DEEPSOLVER_CONFIG_FILE_H
