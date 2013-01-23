/*
   Copyright 2011-2013 ALT Linux
   Copyright 2011-2013 Michael Pozhidaev

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

  bool operator ==(const Lit& lit) const
  {
    return varId == lit.varId && neg == lit.neg;
  }

  bool operator !=(const Lit& lit) const
  {
    return varId != lit.varId || neg != lit.neg;
  }

  bool operator <(const Lit& lit) const
  {
    if (neg != lit.neg)
      return neg;
    if (varId == lit.varId)
      return 0;
    bool res = varId < lit.varId;
    if (neg)
      return !res;
    return res;
  }

  bool operator >(const Lit& lit) const
  {
    if (neg != lit.neg)
      return !neg;
    if (varId == lit.varId)
      return 0;
    bool res = varId > lit.varId;
    if (neg)
      return !res;
    return res;
  }

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
  virtual bool solve(const VarIdToBoolMap& assumptions, VarIdToBoolMap& res, VarIdVector& conflicts) = 0;
}; //class AbstractSatSolver;

std::auto_ptr<AbstractSatSolver> createLibMinisatSolver();

#endif //DEEPSOLVER_ABSTRACT_SAT_SOLVER_H;
