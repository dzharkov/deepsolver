/*
   Copyright 2011-2012 ALT Linux
   Copyright 2011-2012 Dmitry V. Levin
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
#include"RequireFilter.h"
#include"TextFiles.h"

void RequireFilter::load(const std::string& fileName)
{
  std::auto_ptr<AbstractTextFileReader> reader = createTextFileReader(TextFileStd, fileName);
  std::string line;
  while(reader->readLine(line))
    {
      std::string::size_type k = 0;
      while(k < line.length() && BLANK_CHAR(line[k]))
	k++;
      if (k >= line.length())
	continue;
      if (line[k] == '#')
	continue;
      m_requiresToExclude.push_back(trim(line));
    }
}

bool RequireFilter::excludeRequire(const std::string& requireEntry) const
{
  for(StringVector::size_type i = 0;i < m_requiresToExclude.size();i++)
    if (m_requiresToExclude[i] == requireEntry)
      return 1;
  return 0;
}
