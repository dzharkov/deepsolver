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
#include"Repository.h"
#include"utils/TinyFileDownload.h"

void Repository::fetchInfoAndChecksum()
{
  const std::string infoFileUrl = buildInfoFileUrl();
  logMsg(LOG_DEBUG, "Fetching tiny file \'%s\'", infoFileUrl.c_str());
  TinyFileDownload download;//FIXME:max file size limit;
  download.fetch(infoFileUrl);
  m_info = download.getContent();
}

void Repository::addIndexFilesForFetch(StringToStringMap& files)
{
  //FIXME:
}

void Repository::addPackagesToScope(const StringToStringMap& files, PackageScopeContentBuilder& content)
{
  //FIXME:
}

std::string Repository::buildInfoFileUrl() const
{
  assert(!m_url.empty());
  return m_url[m_url.size() - 1] == '/'?m_url + "base/info":m_url + "/base/info";
}
