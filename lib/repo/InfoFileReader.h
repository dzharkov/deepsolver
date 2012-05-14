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

#ifndef DEEPSOLVER_INFO_FILE_READER_H
#define DEEPSOLVER_INFO_FILE_READER_H

enum {
  InfoFileErrorUnexpectedCharacter = 0,
  InfoFileErrorIncompleteLine = 1
};

/**\brief The info file parsing error
 *
 * This class instance is used to notify about any error during
 * repository infor file parsing. In contrast to ConfigFileException the
 * instances of InfoFileException never go to client applications and
 * purposed only for internal information sending.
 *
 * \sa InfoFileReader
 */
class InfoFileException: public DeepsolverException
{
public:
  /**\brief The constructor
   *
   * \param [in] code The error code
   * \param [in] lineNumber The number of the invalid line
   * \param [in] line The invalid line content
   */
  InfoFileException(int code,
		    size_t lineNumber,
		    const std::string& line)
    : m_code(code),
      m_lineNumber(lineNumber),
      m_line(line) {}

  /**\brief The destructor*/
  virtual ~InfoFileException() {}

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
   * Use this method to get content of the line caused the configuration problem.
   *
   * \return The line caused problem
   */
  std::string getLine() const
  {
    return m_line;
  }

  /**\brief Returns the string type of exception
   *
   * This method always returns "info file" string.
   *
   * \return The type of exception (always "info file")
   */
  std::string getType() const
  {
    return "info file";
  }

  /**\brief Returns the single-line configuration error description
   *
   * Using of these method is recommended only for debug purposes.
   *
   * \return The single-line error description
   */
  std::string getMessage() const
  {
    std::ostringstream ss;
    switch(m_code)
      {
      case InfoFileErrorUnexpectedCharacter:
	ss << "unexpected character at line ";
	  break;
      case InfoFileErrorIncompleteLine:
	ss << "incomplete line ";
	break;
      default:
	assert(0);
      } //switch(m_code);
    ss << m_lineNumber << ": " << m_line;
    return ss.str();
  }

private:
  const int m_code;
  const size_t m_lineNumber;
  const std::string m_line;
}; //class InfoFileException;

//FIXME:exception;

class InfoFileReader
{
public:
  InfoFileReader():
    m_currentLineNumber(0) {}

  virtual ~InfoFileReader() {}

public:
  void read(const std::string& text, StringToStringMap& res);

private:
  void parseLine(const std::string& line, std::string& name, std::string& value);

private:
  size_t m_currentLineNumber;
}; //class InfoFileReader;

#endif //DEEPSOLVER_INFO_FILE_READER_H;
