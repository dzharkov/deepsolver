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
#include"utils/CurlInterface.h"
#include<curl/curl.h>

static bool curlWasInitialized = 0;

static int curlProgress(void* p,
			double dlTotal,
			double dlNow,
			double ulTotal,
			double ulNow)
{
  AbstractCurlProgressListener* progressListener = (AbstractCurlProgressListener*)p;
  assert(progressListener != NULL);
  const size_t now = (size_t)dlNow;
  const size_t total = (size_t)dlTotal;
  if (total == 0)
    return 0;
  if (progressListener->onCurlProgress(now, total))
    return 0;
  return 1;
}

static size_t acceptCurlData(void* buf,
			     size_t size,
			     size_t nMemB,
			     void* param)
{
  assert(param != NULL);
  assert(buf != NULL);
  AbstractCurlDataRecipient* recipient = (AbstractCurlDataRecipient*)param;
  return recipient->onNewDataBlock(buf, size * nMemB);
}

void curlInitialize()
{
  if (curlWasInitialized)
    {
      logMsg(LOG_DEBUG, "Curl is already initialized");
      return;
    }
  curl_global_init(CURL_GLOBAL_ALL);
  curlWasInitialized = 1;
  logMsg(LOG_DEBUG, "Curl was initialized");
}

void CurlInterface::init()
{
  CURL* handle = curl_easy_init();
  assert(handle != NULL);
  m_handle = handle;
  logMsg(LOG_DEBUG, "Created new curl object");
}

void CurlInterface::close()
{
  if (m_handle == NULL)
    return;
  CURL* handle = (CURL*)m_handle;
  assert(handle != NULL);
  curl_easy_cleanup(handle);
  m_handle = NULL;
}

void CurlInterface::fetch(const std::string& url,
			  AbstractCurlDataRecipient& recipient,
			  AbstractCurlProgressListener& progressListener)
{
  assert(!url.empty());
  CURL* handle = (CURL*)m_handle;
  assert(handle != NULL);
  curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
  //Uncomment the following line if you want to see debug messages from libcurl on your console;
  //curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L);
  curl_easy_setopt(handle, CURLOPT_PROGRESSFUNCTION, curlProgress);
  curl_easy_setopt(handle, CURLOPT_PROGRESSDATA, &progressListener);
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, acceptCurlData);
  curl_easy_setopt(handle, CURLOPT_FILE, &recipient);
  const CURLcode res = curl_easy_perform(handle);
  if (res)
    throw CurlException(res, url, curl_easy_strerror(res));
}

