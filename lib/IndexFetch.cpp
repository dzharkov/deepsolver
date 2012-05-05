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
#include"IndexFetch.h"

void IndexFetch::fetch(const StringToStringMap& files)
{
  //FIXME:
}

void IndexFetch::processFile(const std::string& url, const std::string localFile)
{
  assert(!url.empty());
  assert(!localFile.empty());
  curlInitialize();//Don't worry about multiple calls of this, there is the check inside;
  m_file.create(localFile);
  CurlInterface curl;
  curl.fetch(url, *this, *this);
}

size_t IndexFetch::onNewDataBlock(const void* buf, size_t bufSize)
{
  //FIXME:
}

bool IndexFetch::onCurlProgress(size_t now, size_t total)
{
  //FIXME:
}
