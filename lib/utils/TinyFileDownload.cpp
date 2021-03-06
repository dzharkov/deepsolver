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
#include"TinyFileDownload.h"

void TinyFileDownload::fetch(const std::string& url)
{
  assert(!url.empty());
  m_content.erase();
  curlInitialize();//Don't worry about multiple calls of this, there is the check inside;
  CurlInterface curl;
  curl.init();
  curl.fetch(url, *this, *this);
  curl.close();
}

size_t TinyFileDownload::onNewDataBlock(const void* buf, size_t bufSize)
{
  assert(buf != NULL);
  const char* c = (const char*)buf;
  std::string s;
  s.resize(bufSize);
  for(size_t i = 0;i < bufSize;i++)
    s[i] = c[i];
  m_content += s;
  return bufSize;
}

bool TinyFileDownload::onCurlProgress(size_t now, size_t total)
{
  return 1;//1 means not to interrupt fetching;
}
