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
#include"RegExp.h"

void RegExp::compile(const std::string& exp)
{
  assert(!exp.empty());
  assert(m_re == NULL);
  m_re = new regex_t;
  const int err = regcomp(m_re, exp.c_str(), REG_EXTENDED | REG_NOSUB);
  if (err != 0)
    {
      char buf[512];
      regerror(err, m_re, buf, sizeof(buf) - 1);
      close();
      throw RegExpException(buf);
    }
}

bool RegExp::match(const std::string& line)
{
  return regexec(m_re, line.c_str(), 0, NULL, 0) != REG_NOMATCH;
}

void RegExp::close()
{
  if (m_re == NULL)
    return;
  regfree(m_re);
  delete m_re;
}
