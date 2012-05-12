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

#ifndef DEEPSOLVER_SAT_H
#define DEEPSOLVER_SAT_H

struct Clause
{
  Clause()
    : var1(BAD_VAR_ID),
      var2(BAD_VAR_ID),
      neg1(0),
      neg2(0) {}

  Clause(bool n1, VarId v1, bool n2, VarId v2)
    : var1(v1),
      var2(v2),
      neg1(n1),
      neg2(n2)  {}

  Clause(VarId v1, VarId v2)
    : var1(v1),
      var2(v2),
      neg1(0),
      neg2(0) {}

  VarId var1, var2;
  bool neg1, neg2;
}; //struct Clause;

typedef std::vector <Clause> ClauseVector;
typedef std::list<Clause> ClauseList;

void solve2Sat(const ClauseVector& clauses, BoolVector& res);

#endif //DEEPSOLVER_SAT_H;
