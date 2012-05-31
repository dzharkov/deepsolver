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
#include"Pkg.h"

std::ostream& operator <<(std::ostream& s, const PkgBase& p)
{
  s << p.name << "-";
  if (p.epoch > 0)
    s << p.epoch << ":";
  s << p.version << "-" << p.release;
  return s;
}

std::ostream& operator <<(std::ostream& s, const NamedPkgRel& r)
{
  s << r.pkgName;
  if (r.ver.empty())
    return s;
  const bool less = r.type & VerLess, equals = r.type & VerEquals, greater = r.type & VerGreater;
  assert(!less || !greater);
  std::string t;
  if (less)
    t += "<";
  if (equals)
    t += "=";
  if (greater)
    t += ">";
  s << " " << t << " " << r.ver;
  return s;
}
