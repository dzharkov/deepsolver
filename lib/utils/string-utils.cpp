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

bool hasNonSpaces(const std::string& s)
{
  for(std::string::size_type i = 0;i < s.length();i++)
    if (!BLANK_CHAR(s[i]))
return 1;
return 0;
}

bool stringBegins(const std::string& str, const std::string& headToCheck, std::string& tail)
{
  assert(!headToCheck.empty());
  if (str.length() < headToCheck.length() || str.find(headToCheck) != 0)
    return 0;
  tail = str.substr(headToCheck.length());
  return 1;
}

bool checkExtension(const std::string& fileName, const std::string& extension)
{
  if (fileName.length() < extension.length())
    return 0; 
  for(std::string::size_type i = 0;i < extension.length();i++)
    if (extension[i] != fileName[fileName.length() - extension.length() + i])
      return 0;
  return 1;
}

std::string trim(const std::string& str)
{
  std::string::size_type l1=0, l2 = str.length();
  while(l1 < str.length() && BLANK_CHAR(str[l1]))
    l1++;
  while(l2 > l1 && BLANK_CHAR(str[l2-1]))
    l2--;
  std::string newStr;
  for(std::string::size_type i = l1;i < l2;i++)
    newStr += str[i];
  return newStr;
}

void splitBySpaces(const std::string& str, StringVector& res)
{
  res.clear();
  std::string s;
  for(std::string::size_type i = 0;i < str.length();i++)
    {
      if (BLANK_CHAR(str[i]))
	{
	  if (s.empty())
	    continue;
	  res.push_back(s);
	  s.erase();
	  continue;
	}
      s += str[i];
    }
  if (!s.empty())
    res.push_back(s);
}
