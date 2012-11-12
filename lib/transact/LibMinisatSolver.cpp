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
  LibMinisatSolver() 
    : m_nextFreeVar(1) {}

  virtual ~LibMinisatSolver() {reset();}

public:
  void reset();
  void addClause(const Clause& clause);
  bool solve(VarIdToBoolMap& res);

private:
  int mapVarId(VarId varId);

private:
  typedef std::map<VarId, int> VarIdToIntMap;
  typedef std::map<int, VarId> IntToVarIdMap;

private:
  int m_nextFreeVar;
  VarIdToIntMap m_varIdToIntMap;
  IntToVarIdMap m_intToVarIdMap;
  std::vector<int*> m_clauses;
  SizeVector m_clauseLengths;
}; //class LibMinisatSolver;

void LibMinisatSolver::reset()
{
  for(size_t i = 0;i < m_clauses.size();i++)
    {
      assert(m_clauses[i] != NULL);
      delete[] m_clauses[i];
    }
  m_clauses.clear();
  m_clauseLengths.clear();
  m_varIdToIntMap.clear();
  m_intToVarIdMap.clear();
  m_nextFreeVar = 1;
}

void LibMinisatSolver::addClause(const Clause& clause)
{
  if (clause.empty())
    return;
  m_clauses.push_back(new int[clause.size()]);
  int* v = m_clauses.back();
  for(Clause::size_type i = 0;i < clause.size();i++)
    if (clause[i].neg)
      v[i] = -1 * mapVarId(clause[i].varId); else
      v[i] = mapVarId(clause[i].varId);
  m_clauseLengths.push_back(clause.size());
}

bool LibMinisatSolver::solve(VarIdToBoolMap& res)
{
  assert(m_clauses.size() == m_clauseLengths.size());
  assert(!m_clauses.empty());
  const size_t varCount = m_nextFreeVar;//Should be on one more than real variable count;
  std::auto_ptr<AutoArrayAdapter<size_t> > counts(new AutoArrayAdapter<size_t>(new size_t[m_clauses.size()]));
  size_t* usingCounts = counts.get()->get();
  std::auto_ptr<AutoArrayAdapter<int*> > eq(new AutoArrayAdapter<int*>(new int*[m_clauses.size()]));
  int** usingEq = eq.get()->get();
  std::auto_ptr<AutoArrayAdapter<unsigned char> > solution(new AutoArrayAdapter<unsigned char>(new unsigned char[m_nextFreeVar]));
  unsigned char* usingSolution = solution.get()->get();
  for(SizeVector::size_type i = 0;i < m_clauseLengths.size();i++)
    {
      usingCounts[i] = m_clauseLengths[i];
      usingEq[i] = m_clauses[i];
    }
  logMsg(LOG_DEBUG, "Calling libminisat to solve the task with %zu variables in %zu clauses", varCount, m_clauses.size());
  if (!minisat_solve(m_clauses.size(), usingCounts, usingEq, usingSolution) != MINISAT_OK)
    {
      logMsg(LOG_DEBUG, "libminisat said no solution!");
      return 0;
    }
  logMsg(LOG_DEBUG, "libminisat found a solution!");
  for(size_t i = 0;i < varCount;i++)
    {
      IntToVarIdMap::const_iterator it = m_intToVarIdMap.find(i);
      assert(it != m_intToVarIdMap.end());
      res.insert(VarIdToBoolMap::value_type(it->second, usingSolution[i] != 0));
    }
  return 1;
}

int LibMinisatSolver::mapVarId(VarId varId)
{
  assert(varId != BAD_VAR_ID);
  VarIdToIntMap::const_iterator it = m_varIdToIntMap.find(varId);
  if (it != m_varIdToIntMap.end())
    {
      assert(it->second != 0);
      return it->second;
    }
  const int newValue = m_nextFreeVar;
  m_varIdToIntMap.insert(VarIdToIntMap::value_type(varId, newValue));
  m_intToVarIdMap.insert(IntToVarIdMap::value_type(newValue, varId));
  m_nextFreeVar++;
  return newValue;
}

std::auto_ptr<AbstractSatSolver> createLibMinisatSolver()
{
  //FIXME:  return auto_ptr<AbstractSatSolver>(new LibMinisatSolver());
  return std::auto_ptr<AbstractSatSolver>();
}

