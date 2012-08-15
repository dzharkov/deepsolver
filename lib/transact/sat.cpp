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

struct Link
{
  Link()
    : var1(BAD_VAR_ID), var2(BAD_VAR_ID) {}

  Link(VarId v1, VarId v2)
    : var1(v1), var2(v2) {}

  VarId var1, var2;
}; //struct Link;

struct Graph
{
  VarId* vertices;
  Link* links;
}; //struct Graph;

/*
void solve2Sat(const ClauseVector& clauses, BoolVector& res)
{
  VarId maxVar = 0;
  for(ClauseVector::size_type i = 0;i < clauses.size();i++)
    {
      assert(clauses[i].var1 != BAD_VAR_ID);
      assert(clauses[i].var2 != BAD_VAR_ID);
      if (clauses[i].var1 > maxVar)
	maxVar = clauses[i].var1;
      if (clauses[i].var2 > maxVar)
	maxVar = clauses[i].var2;
    }
}
*/
