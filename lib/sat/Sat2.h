
#ifndef DEPSOLVER_SAT2_H
#define DEPSOLVER_SAT2_H

#include"Sat.h"

/**\brief The SAT with clauses containing one or two literals*/
class Sat2: public Sat
{
 private:
  struct Clause
  {
    Clause()
      : packageId1(BAD_PACKAGE_ID),
	packageId2(BAD_PACKAGE_ID),
	negative1(0),
	negative2(0) {}

    PackageId packageId1;
    PackageId packageId2;
    vool negative1;
    bool negative2;
  }; //struct Clause;

private:
  typedef std::vector<Clause> ClauseVector;
  typedef std::list<Clause> ClauseList;

public:
  Sat2() {}
  virtual ~Sat2() {}

public:
  size_t getClauseCount() const
  {
    return m_clauses.size();
  }

  size_t getLiteralCount(size_t clauseIndex) const
  {
    assert(clauseIndex < m_clauses.size());
    if (m_clauses[clauseIndex].packageId1 == BAD_PACKAGE_ID)
      return 0;
    if (m_clauses[clauseIndex].packageId2 == BAD_PACKAGE_ID)
      return 1;
    return 2;
  }

  bool empty() const
  {
    return m_clauses.empty();
  }

  void clear()
  {
    m_clauses.clear();
  }

  size_t addEmptyClause();
  void removeClause(size_t clauseIndex);
  vPackageId getPackageId(size_t clauseIndex, size_t literalIndex) const;
  bool isNegative(size_t clauseIndex, size_t literalIndex) const;
  void setLiteral(size_t clauseIndex, size_t literalIndex, PackageId packageId, bool negative = 0);
  void setNegative(size_t clauseIndex, size_t literalIndex, bool value = 0);
  size_t addLiteral(size_t clauseIndex, PackageId packageId, bool negative = 0);
  void removeLiteral(size_t clauseIndex, size_t literalIndex);
}; //class Sat2;

#endif //DEPSOLVER_SAT_H;
