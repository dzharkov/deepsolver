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
#include"AttachedRepo.h"

void AttachedRepo::makeId()
{
  assert(!url.empty());
  assert(!component.empty());
  id = url;
  for(std::string::size_type i = 0;i < id.length();i++)
    {
      const char c = id[i];
      if (c >= 'a' && c <= 'z')
	continue;
      if (c >= 'A' && c <= 'Z')
	continue;
      if (c >= '0' && c <= '9')
	continue;
      id[i] = '_';
    }
}
