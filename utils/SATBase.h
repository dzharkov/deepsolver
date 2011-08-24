
#ifndef __APT_NG_SATBASE_H__
#define __APT_NG_SATBASE_H__

#include<vector>
#include<iostream>
#include"Package.h"

typedef size_t SATVarId;
#define BAD_SAT_VAR_ID (SATVarId)-1

struct Literal
{
  Literal(): varId(BAD_SAT_VAR_ID), negative(0) {}
  Literal(SATVarId value): varId(value), negative(0) {}
  Literal(SATVarId value, bool neg): varId(value), negative(neg) {}

  SATVarId varId;
  bool negative;
}; //struct Literal;

typedef std::vector<Literal> Clause;

inline Clause unitClause(SATVarId varId, bool negative = 0)
{
  Clause c;
  c.push_back(Literal(varId, negative));
  return c;
}

typedef std::list<Clause> SAT;

void printLiteral(std::ostream& s, const PackageVector& packages, const Literal& l);
void printClause(std::ostream& s, const PackageVector& packages, const Clause& clause);
void printSAT(std::ostream& s, const PackageVector& packages, const SAT& sat);

#endif //__APT_NG_SATBASE_H__;
