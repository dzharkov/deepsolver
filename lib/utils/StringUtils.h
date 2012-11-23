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

#ifndef DEEPSOLVER_STRING_UTILS_H
#define DEEPSOLVER_STRING_UTILS_H

#define BLANK_CHAR(x) ((x)==10 || (x)==13 || (x)==9 || (x)==32)

bool hasNonSpaces(const std::string& s);
bool stringBegins(const std::string& str, const std::string& headToCheck, std::string& tail);
bool checkExtension(const std::string& fileName, const std::string& extension);
std::string trim(const std::string& str);
void splitBySpaces(const std::string& str, StringVector& res);

#endif //DEEPSOLVER_STRING_UTILS_H
