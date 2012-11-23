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

#ifndef DEEPSOLVER_DEEPSOLVER_EXCEPTIONS_H
#define DEEPSOLVER_DEEPSOLVER_EXCEPTIONS_H

#define TRY_SYS_CALL(expr, msg) if (!(expr)) throw SystemException(msg)
#define SYS_STOP(msg) throw SystemException(msg)

/**\brief The main exception class for Deepsolver project
 *
 * The every exception class used for error indication in Deepsolver
 * project must be the child (not strongly directly) of this
 * DeepsolverException class. It is created to simplify error handling and
 * make it unified. The main information this class must provide is the
 * error type and single line error description.
 *
 * \sa SystemException RpmException IndexCoreException ConfigException ConfigFileException CurlException InfoFileException OperationException TaskException
 */
class DeepsolverException
{
public:
  /**\brief The default constructor*/
  DeepsolverException() {}

  /**\brief the destructor*/
  virtual ~DeepsolverException() {}

public:
  /**\brief Returns the string with error type
   *
   * This method returns the short string with one or two words describing
   * the error type. For example, this method can return values like
   * "system", "rpm" etc. The value returned by this method usually is used
   * for error message construction.
   *
   * \return The short string with error type description
   */
  virtual std::string getType() const = 0;

  /**\brief Returns the single line error description
   *
   * This method returns the single line string value with error
   * description. Usually it is the value printed to user in error
   * message. The value may not include error type since it can be obtained
   * with getType() method.
   *
   * \return The single line error description
   */
  virtual std::string getMessage() const = 0;
}; //class DeepsolverException;

/**\brief The exception for system call errors
 *
 * This class is used for indication of errors caused by various system
 * calls problems. It automatically analyzes value of system errno
 * variable and can construct informative error description with text
 * provided by operating system about error occurred. The error message
 * consists of two parts: the short string provided by developer with any
 * information hi want and the string given by the operating system. The
 * developer can save in the string, for example, the name of system call
 * with its arguments which execution failed.
 *
 * \sa Exception
 */
class SystemException: public DeepsolverException
{
public:
  /**\brief The default constructor
   *
   * This constructor implies automatic errno analyzing but without
   * developer string, so the getMessage() method will return only the
   * single line description provided by operating system.
   */
  SystemException()
    : m_code(errno) {}

  /**\brief The constructor with automatic errno analyzing with developer comment
   *
   * This constructor allows developer to give short comment of the error
   * and automatically add string from operating system got with errno
   * variable.
   *
   * \param [in] comment The developer error comment*
   */
  SystemException(const std::string& comment)
    : m_code(errno), m_comment(comment) {}

  /**\brief The constructor with error code specification
   *
   * Using this constructor the developer can provide error code by himself
   * without any additional comment. The operating system will be requested
   * for error description using provided error code. The developer must
   * give the value usually got through errno variable.
   *
   * \param [in] code The error code
   */
  SystemException(int code)
    : m_code(code) {}

  /**\brief The constructor with error code and comment specification
   *
   * With this constructor developer can provide the error code to request
   * description about from operation system and any short comment he
   * want. The value of error code must be the value usually taken through
   * errno variable.
   * \param [in] code The error code
   * \param [in] comment The developer error additional information
   */
  SystemException(int code, const std::string& comment)
    : m_code(code), m_comment(comment) {}

  /**\brief The destructor*/
  virtual ~SystemException() {}

public:
  /**\brief Returns the type of an error
   *
   * This method always returns the "system" string. This string is used in
   * error message construction and allows to distinguish the system errors
   * from other types of exceptions.
   *
   * \return The single word exception type ("system")
   */
  std::string getType() const
  {
    return "system";
  }

  /**\brief Returns the error code
   *
   * This method returns the errno value got at exception creations.
   *
   * \return The erno value of corresponding error
   */
  int getCode() const
  {
    return m_code;
  }

  /**\brief The error description from the operating system
   *
   * This method returns the error description from the operating system
   * got by the errno value with strerror[] array.
   *
   * \return The error description from the operating system
   */
  std::string getDescr() const
  {
    return strerror(m_code);
  }

  /**\brief Returns the developer error comment
   *
   * This method returns the error comment provided by developer.
   *
   * \return The developer error description
   */
  std::string getComment() const
  {
    return m_comment;
  }

  std::string getMessage() const
  {
    return getComment()+":"+getDescr();
  }

private:
  int m_code;
  std::string m_comment;
}; //class SystemException;

enum {
  OperationErrorInvalidInfoFile = 0,
  OperationErrorInvalidChecksum = 1,
  OperationErrorBrokenIndexFile = 2,
OperationErrorInternalIOProblem = 3
};

enum {
  TaskErrorUnknownPackageName = 1,
  TaskErrorBothInstallRemove = 2,
  TaskErrorNoInstallationCandidat = 3
};

//This class must have i18n support in the future;
class TaskException: public DeepsolverException
{
public:
 TaskException(int code, const std::string& arg)
   : m_code(code), m_arg(arg) {}

  virtual ~TaskException() {}

public:
  int getCode() const
  {
    return m_code;
  }

  const std::string& getArg() const
  {
    return m_arg;
  }

  std::string getType() const
  {
    return "task";
  }

  std::string getMessage() const
  {
    switch(m_code)
      {
      case TaskErrorUnknownPackageName:
	return "unknown package name: " + m_arg;
      case TaskErrorBothInstallRemove:
	return "conflicting actions: \'" + m_arg + "\' was considered both for install and remove";
      case TaskErrorNoInstallationCandidat:
	return "package has no installation candidat: " + m_arg;
      default:
	assert(0);
	return "";//Just to reduce warning 
      }; //switch(m_code);
  }

private:
  const int m_code;
  const std::string m_arg;
}; //class TaskException;

/**\brief The exception class for general operation problems
 *
 * This class is purposed for various general operation problems.
 * The errors can be thrown only by the methods of
 * OperationCore class covering transaction processing as well as index
 * updating. General error types are checksum
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

  /**\brief The destructor*/
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

  std::string getType() const
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
 * This class instance is thrown when configuration file syntax error is
 * encountered. The client application can access various information
 * with it about the problem like file name, line number, character
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

  std::string getType() const
  {
    return "config syntax";
  }

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

enum {
  ConfigErrorUnknownParam = 0,
  ConfigErrorValueCannotBeEmpty = 1,
  ConfigErrorAddingNotPermitted = 2,
ConfigErrorInvalidBooleanValue = 3
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
   * \param [in] fileName The name of the config file with the invalid line
   * \param [in] lineNumber The number of the invalid line
   * \param [in] line The invalid line value
   */
  ConfigException(int code,
		  const std::string& arg,
		  const std::string& fileName,
		  size_t lineNumber,
		  const std::string& line)
    : m_code(code),
      m_arg(arg),
      m_fileName(fileName),
      m_lineNumber(lineNumber),
      m_line(line) {}

  /**\brief The constructor with no configuration file line info
   *
   * \param [in] code The error code
   * \param [in] arg Optional argument with meaning dependent on error code
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

  std::string getType() const
  {
    return "configuration";
  }

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
    ss << "Info file syntax error at line " << m_lineNumber << ":";
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
    return ss.str();
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


class CurlException: public DeepsolverException
{
public:
  CurlException(int code, const std::string& url, const std::string& text)
    : m_code(code), m_url(url), m_text(text) {}

  virtual ~CurlException() {}

public:
  int getCode() const
  {
    return m_code;
  }

  std::string getUrl() const
  {
    return m_url;
  }

  std::string getText() const
  {
    return m_text;
  }

  std::string getType() const
  {
    return "curl";
  }

  std::string getMessage() const
  {
    return m_url + ":" + m_text;
  }

private:
  const int m_code;
  const std::string m_url;
  const std::string m_text;
}; //class CurlException;

class RegExpException: public DeepsolverException
{
public:
  /**\brief The constructor
   *
   * \param [in] message The error message
   */
 RegExpException(const std::string& message)
    : m_message(message) {}

  /**\brief The destructor*/
  virtual ~RegExpException() {}

public:
  std::string getType() const
  {
    return "regular expression";
  }

  std::string getMessage() const
  {
    return "Invalid regular expression:" + m_message;
  }

private:
  const std::string m_message;
}; //class RegExpException;




/**\brief The exception class for md5file syntax errors
 *
 * This class instance is thrown when md5file syntax error isCon
 * encountered. The client application can access various information
 * with it about the problem like file name, line number,
 * line content and error code.
 *
 * \sa Md5File Md5
 */
class Md5FileException: public DeepsolverException
{
public:

  enum {
    TooShortLine = 0,
    InvalidChecksumFormat = 2
  };

public:
  /**\brief The constructor
   *
   * \param [in] code The error code describing the type of error
   * \param [in] fileName Name of a file being processed
   * \param [in] lineNumber The number of a line with encountered error
   * \param [in] line The text of an invalid line
   */
  Md5FileException(int code,
		      const std::string& fileName,
		      size_t lineNumber,
		      const std::string& line)
    : m_code(code),
      m_fileName(fileName),
      m_lineNumber(lineNumber),
      m_line(line) {}

  /**\brief The destructor*/
  virtual ~Md5FileException() {}

public:
  /**\brief Returns the type of encountered error
   *
   * Use this method to get type of an error.
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

  /**\brief Returns number of an invalid line in file
   *
   * Use this method to get number of line with the syntax problem.
   *
   * \return Number of an invalid line
   */
  size_t getLineNumber() const
  {
    return m_lineNumber;
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

  std::string getType() const
  {
    return "md5file";
  }

  std::string getMessage() const
  {
    std::string msg;
    switch(m_code)
      {
      case TooShortLine:
	msg = "too short line";;
	break;
      case InvalidChecksumFormat:
	msg = "invalid checksum format";
	break;
      default:
	assert(0);
      };
    std::ostringstream ss;
    ss << m_fileName << "(" << m_lineNumber << "):" << msg << ":" << m_line;
    return ss.str();
  }

private:
  const int m_code;
  const std::string m_fileName;
  const size_t m_lineNumber;
  const std::string m_line;
}; //class Md5FileException;

class RpmException: public DeepsolverException
{
public:
  RpmException() {}
  RpmException(const std::string& message)
    : m_message(message
) {}

      virtual ~RpmException() {}

public:
  std::string getType() const
  {
    return "rpm";
  }

  std::string getMessage() const
  {
    return m_message;
  }

private:
  std::string m_message;
}; //class RpmException;

#endif //DEEPSOLVER_DEEPSOLVER_EXCEPTIONS_H;
