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
#include"TextFormatSectionReader.h"

#define IO_BUF_SIZE 2048

void AbstractTextFormatSectionReader::init()
{
  logMsg(LOG_DEBUG, "Initializing TextFormatSectionReader");
  m_noMoreData = 0;
  m_queue.erase();
  char buf[IO_BUF_SIZE];
  size_t bracketPos, readCount;
  while(1)
    {
      readCount = readData(buf, IO_BUF_SIZE);
      //      logMsg(LOG_DEBUG, "Read chunk of data, length is %zu bytes", readCount);
      if (readCount == 0)
	{
	  m_noMoreData = 1;
	  return;
	}
      bracketPos = 0;
      while(bracketPos < readCount && buf[bracketPos] != '[')
	bracketPos++;
      //      logMsg(LOG_DEBUG, "bracketPos = %zu", bracketPos);
      if (bracketPos < readCount)
	break;
    }
  assert(buf[bracketPos] == '[');
  while(bracketPos < readCount)
    m_queue += buf[bracketPos++];
  logMsg(LOG_DEBUG, "TextFormatSectionReader initialized with queue of %zu characters", m_queue.size());
}

bool AbstractTextFormatSectionReader::readNext(std::string& s)
{
  if (m_noMoreData)
    return 0;
  assert(!m_queue.empty() && m_queue[0] == '[');
  std::string::size_type pos = 1;
  while(1)
    {
      assert(pos > 0);
      while(pos < m_queue.size() && (m_queue[pos - 1] != '\n' || m_queue[pos] != '['))
	pos++;
      if (pos < m_queue.size())//New section header found;
	break;
      char buf[IO_BUF_SIZE];
      const size_t readCount = readData(buf, IO_BUF_SIZE);
      if (readCount == 0)
	{
	  m_noMoreData = 1;
	  break;
	}
      for(size_t i = 0;i < readCount;i++)
	m_queue += buf[i];
    }
  assert (pos < m_queue.size() || m_noMoreData);
  assert(pos > 0);
  assert(pos >= m_queue.size() || (m_queue[pos - 1] == '\n' && m_queue[pos] == '['));
  if (pos >= m_queue.size())
    {
      assert(m_noMoreData);
      s = m_queue;
      m_queue.erase();
      return 1;
    }
  s = m_queue.substr(0, pos);
  m_queue = m_queue.substr(pos);
  assert(!m_queue.empty() && m_queue[0] == '[');
  return 1;
}
