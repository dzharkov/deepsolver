/*
   Copyright 2011-2013 ALT Linux
   Copyright 2011-2013 Michael Pozhidaev

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
#include"IndexFetch.h"

void IndexFetch::fetch(const StringToStringMap& files)
{
  curlInitialize();
  m_lastPartPercents = 0;
  m_lastTotalPercents = 0;
  m_partCount = files.size();
  m_currentPartNumber = 0;
  for(StringToStringMap::const_iterator it = files.begin();it != files.end();it++)
    {
      logMsg(LOG_DEBUG, "Fetching of \'%s\'", it->first.c_str());
      m_currentFileName = it->first;
      processFile(it->first, it->second);
      m_currentPartNumber++;
    }
    {
    }
}

void IndexFetch::processFile(const std::string& url, const std::string localFile)
{
  assert(!url.empty());
  assert(!localFile.empty());
  curlInitialize();//Don't worry about multiple calls of this, there is the check inside;
  m_file.create(localFile);
  CurlInterface curl;
  curl.init();
  curl.fetch(url, *this, *this);
  curl.close();
  m_file.close();
}

size_t IndexFetch::onNewDataBlock(const void* buf, size_t bufSize)
{
  assert(m_file.opened());
  m_file.write(buf, bufSize);
  return bufSize;
}

bool IndexFetch::onCurlProgress(size_t now, size_t total)
{
  assert(m_partCount > 0);
  const double inOnePart = 100 / m_partCount;
  double partPercents = ((double)now / total) * 100;
  double percents = (double)m_currentPartNumber * inOnePart + ((double)now / total) * inOnePart;
  if (partPercents > 100)
    partPercents = 100;
  if (percents > 100)
    percents = 100;
  unsigned char intPartPercents = (unsigned char)partPercents;
  unsigned char intPercents = (unsigned char)percents;
  if (intPartPercents > 100)
    intPartPercents = 100;
  if (intPercents > 100)
    intPercents = 100;
  if (intPartPercents == m_lastPartPercents && intPercents == m_lastTotalPercents)
    return 1;
  m_lastPartPercents = intPartPercents;
  m_lastTotalPercents = intPercents;
  m_listener.onIndexFetchStatus(intPartPercents, intPercents, m_currentPartNumber, m_partCount, total, m_currentFileName);
  return m_continueRequest.onContinueOperationRequest();
}
