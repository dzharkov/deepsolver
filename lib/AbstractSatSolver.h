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

#ifndef DEEPSOLVER_SAT_SOLVER_H
#define DEEPSOLVER_SAT_SOLVER_H

struct Lit
{
  Lit()
    : varId(BAD_VAR_ID), neg(0) {}

  Lit(VarId v)
    : varId(v), neg(0) {}

  Lit(VarId v, bool n)
    : varId(v), neg(n) {}

  VarId varId;
  bool neg;
}; //struct Lit;

typedef std::vector<Lit> Clause;
typedef std::vector<Clause> Sat;

inline Clause unitClause(const Lit& lit)
{
  Clause c;
  c.push_back(lit);
  return c;
}

class AbstractSatSolver
{
public:
  typedef std::map<VarId, bool> VarIdToBoolMap;
public:
  AbstractSatSolver() {}
  virtual ~AbstractSatSolver() {}

public:
  virtual void reset() = 0;
  virtual void addClause(const Clause& clause) = 0;
  virtual bool solve(VarIdToBoolMap& res) = 0;
}; //class AbstractSatSolver;

std::auto_ptr<AbstractSatSolver> createLibMinisatSolver();

#endif //DEEPSOLVER_ABSTRACT_SAT_SOLVER_H;
