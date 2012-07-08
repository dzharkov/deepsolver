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

#ifndef DEEPSOLVER_INFO_FILE_WRITER_H
#define DEEPSOLVER_INFO_FILE_WRITER_H

/**\brief Constructs and writes repository information file
 *
 * This class takes a string to string map and saves it as a file using
 * the repository information file format. New file beginning contains
 * additional comment with short purpose and format description.
 *
 * The values of provided string to string map can be strings with any
 * content keeping in mind all blank characters in value beginning and
 * ending will be silently skipped on reading. The map keys can contain
 * only small and capital latin letters, digits, dashes and underlines.
 *
 * \sa IndexCore InfoFileReader TextFormatWriter
 */
class InfoFileWriter
{
public:
  /**\brief The default constructor*/
  InfoFileWriter() {}

  /**\brief The destructor*/
  virtual ~InfoFileWriter() {}

public:
  /**\brief Saves provided map to disk
   *
   * This method throws SystemException on any I/O problems.
   *
   * \param [in] fileName The name of the file to save to
   * \param [in] params The user defined string to string map to save
   */
  void write(const std::string& fileName, const StringToStringMap& params) const;
}; //class InfoFileWriter;

#endif //DEEPSOLVER_INFO_FILE_WRITER_H;
