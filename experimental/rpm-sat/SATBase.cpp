

#include<assert.h>
#include<iostream>
#include"SATBase.h"
#include"Package.h"

void printLiteral(std::ostream& s, const PackageVector& packages, const Literal& l)
{
  if (l.negative)
    s << "!";
  assert(l.varId < packages.size());
  s << packages[l.varId].name;
}

void printClause(std::ostream& s, const PackageVector& packages, const Clause& clause)
{
  if (clause.empty())
    {
      s << "()";
      return;
    }
  if (clause.size() == 1)
    {
      printLiteral(s, packages, clause[0]);
      return;
    }
  s << "(";
  printLiteral(s, packages, clause[0]);
  for(Clause::size_type i = 1;i < clause.size();i++)
    {
      s << " or ";
      printLiteral(s, packages, clause[i]);
    }
  s << ")";
}

void printSAT(std::ostream& s, const PackageVector& packages, const SAT& sat)
{
  if (sat.empty())
    {
      s << "Empty SAT";;
      return;
    }
bool first = 1;
for(SAT::const_iterator it = sat.begin();it != sat.end();++it)
{
if (!first)
s << " and "; else 
first = 0;
printClause(s, packages, *it);
}
}

bool equalClauses(const Clause& c1, const Clause& c2)
{
  if (c1.size() != c2.size())
    return 0;
  for(Clause::size_type i = 0;i < c1.size();i++)
    if (c1[i] != c2[i])
      return 0;
  return 1;
}
