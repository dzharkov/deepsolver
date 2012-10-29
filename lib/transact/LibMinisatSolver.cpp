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
#include"AbstractSatSolver.h"
#include<minisat.h>

class LibMinisatSolver: public AbstractSatSolver
{
public:
  LibMinisatSSolver() {}
  virtual ~LibMinisatSolver() {reset();}

public:
  void reset();
  void addClause(const Clause& clause);
  bool solve(BoolVector& res);

private:
  std::vector<int*> m_clauses;

}; //class LibMinisatSolver;

void LibMinisatSolver::reset()
{

}

void LibMinisatSolver::addClause(const Clause& clause)
{
}

bool LibMinisatSolver::solve(BoolVector& res)
{
}

std::auto_ptr<AbstractSatSolver> createLibMinisatSolver()
{
  return auto_ptr<AbstractSatSolver>(new LibMinisatSolver());
}
