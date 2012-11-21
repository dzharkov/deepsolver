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

/**\brief Stores the information about position in configuration file
 *
 * This structure saves the complete configuration file position
 * information necessary for constructing proper error message. This
 * information is provided to parsing process listening class.
 *
 * \sa AbstractConfigFileHandler ConfigFile
 */
struct ConfigFilePosInfo
{
  /**\brief The constructor
   *
   * \param [in] f The configuration file name
   * \param [in] ln The number of the line with the invalid content
   * \param [in] l The content of the invalid line
   */
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

/**\brief The configuration file format parser
 *
 * This class performs general configuration file syntax parsing but
 * don't take care about configuration data semantics. That means the
 * another handler must be used in addition to this class to process
 * semantics.
 * 
 * This parser doesn't read file from disk by itself. The client
 * implementation should provide content line-by-line receiving parsed
 * data back through special callback interface. However the
 * configuration file name must be provided since it is used in syntax
 * error messages construction. One instance of this parser may be used
 * only for one file because it has internal line counter. By the same
 * reason the empty lines must be given as well as lines with any
 * content.
 *
 * \sa ConfigCenter AbstractConfigHandler ConfigFileException
 */
class ConfigFile
{
private:
  enum {
    ModeAssign = 0,
    ModeAdding = 1
  };

public:
  /**\brief The constructor
   *
   * \param [in] handler The reference to the handler object for parsed data processing
   * \param [in] fileName The name of the file being parsed
   */
  ConfigFile(AbstractConfigFileHandler& handler, const std::string fileName)
    : m_handler(handler),
      m_fileName(fileName),
      m_linesProcessed(0),
      m_sectLevel(0),
      m_sectArgPos(0),
      m_assignMode(ModeAssign) {}

  /**\brief The destructor*/
  virtual ~ConfigFile() {}

public:
  /**\brief Processes one configuration file line
   *
   * Use this method to parse one line and receive parsed data back.
   *
   * \param [in] The content of the line to process
   */
  void processLine(const std::string& line);

private:
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
