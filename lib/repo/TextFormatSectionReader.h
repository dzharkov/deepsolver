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

#ifndef DEEPSOLVER_TEXT_FORMAT_SECTION_READER_H
#define DEEPSOLVER_TEXT_FORMAT_SECTION_READER_H

class TextFormatSectionReader
{
public:
  TextFormatSectionReader(File& file)
    : m_file(file),
      m_noMoreData(0) {}
  virtual ~TextFormatSectionReader() {}

public:
  bool readNext(std::string& s);

private:
  File& m_file;
  bool m_noMoreData;
  std::string m_queue;
}; //class TextFormatSectionReader; 

#endif //DEEPSOLVER_TEXT_FORMAT_SECTION_READER_H;
