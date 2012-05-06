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

#include"deepsolver.h"
#include"InfoFileReader.h"

#define STATE_INIT 0
#define STATE_NAME 1
#define STATE_BEFORE_EQUALS 2
#define STATE_VALUE 3

inline bool validIdentChar(char c)
{
  if (c >= 'a' && c <= 'z')
    return 1;
  if (c >= 'A' && c <= 'Z')
    return 1;
  if (c >= '0' && c <= '9')
    return 1;
  if (c == '_' || c == '-' || c == '+')
    return 1;
  return 0;
}

void InfoFileReader::read(const std::string& text, StringToStringMap& res)
{
  res.clear();
  m_currentLine = 0;
  //FIXME:
}

void InfoFileReader::parseLine(const std::string& line, std::string& name, std::string& value)
{
  int state = STATE_INIT;
  name.erase();
  value.erase();
  for(std::string::size_type i = 0;i < line.length();i++)
    {
      const char c = line[i];
      //Line beginning;
      if (state == STATE_INIT)
	{
	  if (BLANK_CHAR(c))
	    continue;
	  if (c == '#')
	    return;
	  if (validIdentChar(c))
	    {
	      state = STATE_NAME;
	      name += c;
	      continue;
	    }
	  throw InfoFileException(InfoFileErrorUnexpectedCharacter, m_currentLineNumber, line);
	}//Beginning;
      //Name;
      if (state == STATE_NAME)
	{
	  if (validIdentChar(c))
	    {
	      name += c;
	      continue;
	    }
	  if (c == '=')
	    {
	      state = STATE_VALUE;
	      continue;
	    }
	  if (BLANK_CHAR(c))
	    {
	      state = STATE_BEFORE_EQUALS;
	      continue;
	    }
	  if (c == '#')
	    throw InfoFileException(InfoFileErrorIncompleteLine, m_currentLineNUmber);
	  throw InfoFileException(InfoFileErrorUnexpectedCharacter, m_currentLineNumber, line);
	}//Name;
      if (state == STATE_BEFORE_EQUALS)
	{
	  if (BLANK_CHAR(c))
	    continue;
	  if (c == '=')
	    {
	      state = STATE_VALUE;
	      continue;
	    }
	  if (c == '#')
	    throw InfoFileException(InfoFileErrorIncompleteLine, m_currentLineNumber);
	  throw InfoFileException(InfoFileErrorUnexpectedCharacter, m_currentLineNumber, line);
	} //Before equals;
      if (state == STATE_VALUE)
	{
	  if (c != '#' && c != '\\')
	    {
	      value += c;
	      continue;
	    }
	  if (c == '#')
	    return;
	  assert(c == '\\');
	  if (i + 1>= line.length())
	    throw InfoFileException(InfoFileErrorIncompleteLine, m_currentLineNumber);
	  i++;
	  if (line[i] != '#')
	    throw InfoFileException(InfoFileErrorUnexpectedCharacter, m_currentLineNumber);
	  value += '#';
	  continue;
	}//Value;
    }
  if (state != STATE_VALUE)
    throw InfoFileException(InfoFileErrorIncompleteLine, m_currentLineNUmber);
}
