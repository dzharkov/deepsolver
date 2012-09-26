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

#ifndef DEEPSOLVER_INFO_FILE_EXCEPTION_H
#define DEEPSOLVER_INFO_FILE_EXCEPTION_H


/**\brief The general info file error
 * FIXME
 *
 * \sa InfoFileReader RepoParams InfoFileSyntaxEException InfoFileValueException
 */
class InfoFileException: public DeepsolverException
{
public:
  /**brief The default constructor*/
  InfoFileException() {}

  /**\brief The destructor*/
  virtual ~InfoFileException() {}

  //No own methods here;
}; //class InfoFileException;

/**\brief The info file syntax error
 *
 * FIXME
 *
 * \sa InfoFileReaderInfoFileException InfoFileValueException
 */
class InfoFileSyntaxException: public InfoFileException
{
public:
  enum {
    UnexpectedCharacter = 0,
    IncompleteLine = 1
  };

public:
  /**\brief The constructor
   *
   * \param [in] code The error code
   * \param [in] lineNumber The number of the invalid line
   * \param [in] line The invalid line content
   */
  InfoFileSyntaxException(int code,
		    size_t lineNumber,
		    const std::string& line)
    : m_code(code),
      m_lineNumber(lineNumber),
      m_line(line) {}

  /**\brief The destructor*/
  virtual ~InfoFileSyntaxException() {}

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

  /**\brief Returns number of the invalid line
   *
   * Use this method to get number of the invalid line.
   *
   * \return The number of the line caused the problem
   */
  size_t getLineNumber() const
  {
    return m_lineNumber;
  }

  /**\brief Returns the line caused the problem
   *
   * Use this method to get content of the line caused the parsing problem.
   *
   * \return The line caused problem
   */
  std::string getLine() const
  {
    return m_line;
  }

  std::string getType() const
  {
    return "info file syntax";
  }

  std::string getMessage() const
  {
    std::ostringstream ss;
    ss << "Info file syntax error at line " << m_lineNumber + 1 << ":";
    switch(m_code)
      {
      case UnexpectedCharacter:
	ss << "unexpected character";
	  break;
      case IncompleteLine:
	ss << "incomplete line";
	break;
      default:
	assert(0);
      } //switch(m_code);
    ss << ":" << m_line;
  }

private:
  const int m_code;
  const size_t m_lineNumber;
  const std::string m_line;
}; //class InfoFileSyntaxException;

/**\brief The exception class for invalid info file value error indication
 * FIXME
 *
 * \sa InfoFileReader RepoParams InfoFileException InfoFileSyntaxException
 */
class InfoFileValueException: public InfoFileException
{
public:
  enum {
    InvalidFormatType = 0,
    InvalidCompressionType = 1,
    InvalidBooleanValue = 2
  };

public:
  /**\brief The constructor
   *
   * \param [in] code The error code
   * \param [in] arg The string error argument
   */
  InfoFileValueException(int code, const std::string& arg)
    : m_code(code),
      m_arg(arg) {}

  /**\brief The destructor*/
  virtual ~InfoFileValueException() {}

public:
  /**\brief Returns the error code
   *
   * Use this method to get code of the error.
   *
   * \return The error code
   */
  int getCode() const
  {
    return m_code;
  }

  /**\brief Returns the error argument
   *
   * Use this method to get string argument of the error.
   *
   * \return The error argument
   */
   const std::string& getArg() const
   {
   return m_arg;
   } 

   /**\brief Returns the exception type
   *
   *This method always returns "info file value".
   *
   * \sa The exception type
   */
  std::string getType() const
  {
    return "info file value";
  }

  std::string getMessage() const
  {
    std::string s = "Info file value error:";
    switch(m_code)
      {
      case InvalidFormatType:
	return s + "invalid format type: \'" + m_arg + "\'";
      case InvalidCompressionType:
	return s + "invalid compression type: \'" + m_arg + "\'";
      case InvalidBooleanValue:
	return s + "parameter \'" + m_arg + "\' has an invalid boolean value";
      default:
	assert(0);
      }
    return "";
  }

private:
  const int m_code;
  const std::string m_arg;
}; //class InfoFileValueException;

#endif //DEEPSOLVER_INFO_FILE_EXCEPTION_H;
