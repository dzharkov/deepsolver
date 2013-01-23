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

#ifndef DEEPSOLVER_INDEX_FETCH_PROGRESS_H
#define DEEPSOLVER_INDEX_FETCH_PROGRESS_H

#include"OperationCore.h"

class IndexFetchProgress: public AbstractIndexFetchListener
{
public:
  IndexFetchProgress(std::ostream& stream, bool suppressed)
    : m_stream(stream), 
      m_prevStrLen(0),
      m_suppressed(suppressed) {}

  virtual ~IndexFetchProgress() {}

public:
  void onInfoFilesFetch();
  void onIndexFetchBegin();
  void onIndexFilesReading();
  void onIndexFetchComplete();
  void onIndexFetchStatus(unsigned char currentPartPercents,
			  unsigned char totalPercents,
			  size_t partNumber,
			  size_t partCount,
			  size_t currentPartSize,
			  const std::string& currentPartName);

private:
  std::ostream& m_stream;
  std::string::size_type m_prevStrLen;
  bool m_suppressed;
}; //class IndexFetchProgress;

#endif //DEEPSOLVER_INDEX_FETCH_PROGRESS_H;
