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

#ifndef DEEPSOLVER_INDEX_FETCH_H
#define DEEPSOLVER_INDEX_FETCH_H

#include"OperationCore.h"
#include"utils/CurlInterface.h"

class IndexFetch: private AbstractCurlDataRecipient, private AbstractCurlProgressListener
{
public:
  IndexFetch(AbstractIndexFetchListener& listener,   const AbstractOperationContinueRequest& continueRequest)
   : m_listener(listener), m_continueRequest(continueRequest) {}

  virtual ~IndexFetch() {}

 public:
  void fetch(const StringToStringMap& files);

private:
  void processFile(const std::string& url, const std::string localFile);

private://AbstractCurlDataRecipient;
  size_t onNewDataBlock(const void* buf, size_t bufSize);

private://AbstractCurlProgressListener;
  bool onCurlProgress(size_t now, size_t total);

private:
  AbstractIndexFetchListener& m_listener;
  const AbstractOperationContinueRequest& m_continueRequest;
  File m_file;
}; //class IndexFetch;

#endif //__DEEPSOLVER_INDEX_FETCH_H;
