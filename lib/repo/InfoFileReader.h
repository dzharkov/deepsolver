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

/*
 * In contrast to ConfigFileException the instances of 
 * InfoFileException never go to client applications and purposed only for internal information sending.
 */

//FIXME:exception;

class InfoFileReader
{
public:
  InfoFileReader():
    m_currentLine(0) {}

  virtual ~InfoFileReader() {}

public:
  void read(const std::string& text, StringToStringMap& res) const;

private:
  void parseLine(const std::string& line, std::string& name, std::string& value);

private:
  size_t m_currentLine;
}; //class InfoFileReader;

#endif //DEEPSOLVER_INFO_FILE_READER_H;
