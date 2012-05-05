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
#include"IndexFetchProgress.h"

void IndexFetchProgress::onIndexFetchStatus(unsigned char currentPartPercents,
					    unsigned char totalPercents,
					    size_t partNumber,
					    size_t partCount,
					    size_t currentPartSize,
					    const std::string& currentPartName)
{
  assert(totalPercents <= 100);
  m_stream << totalPercents << "% (file " << partNumber << " of " << partCount << ", " << currentPartSize / 1024 << "k, " << currentPartName << ")" << std::endl;
}
