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

#ifndef DEEPSOLVER_REQUIRE_FILTER_H
#define DEEPSOLVER_REQUIRE_FILTER_H

#include"repo/TextFormatWriter.h"

class RequireFilter: public AbstractRequireFilter
{
public:
  RequireFilter() {}
  virtual ~RequireFilter() {}

public:
  void load(const std::string& fileName);
  bool excludeRequire(const std::string& requireEntry) const;

private:
  StringVector m_requiresToExclude;
}; //class RequireFilter;

#endif //DEEPSOLVER_REQUIRE_FILTER_H;
