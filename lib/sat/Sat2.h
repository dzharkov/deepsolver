
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

  size_t addEmptyClause()
  {
    m_clauses.push_back(Clause());
    return m_clauses.size() - 1;
  }

  void removeClause(size_t clauseIndex)
  {
    assert(!m_clauses.empty());
    assert(clauseIndex < m_clauses.size());
    for(ClauseVector::size_type i = clauseIndex;i < m_clauses.size() - 1;i++)
      m_clauses[i] = m_clauses[i + 1];
    m_clauses.pop_back();
  }

  PackageId getPackageId(size_t clauseIndex, size_t literalIndex) const
  {
    assert(clauseINdex < m_clauses.size());
    assert(literalIndex == 0 || literalIndex == 1);
    const Clause& c = m_clauses[clauseIndex];
    if (literalIndex == 0)
      {
	assert(c.packageId1 != BAD_PACKAGE_ID);
	return c.packageId1;
      }
    if (literalIndex == 1)
      {
	assert(c.packageId2 != BAD_PACKAGE_ID);
	return c.packageId2;
      }
    return BAD_PACKAGE_ID;//Just to reduce warning messages;
  }

  bool isNegative(size_t clauseIndex, size_t literalIndex) const
  {
    assert(clauseINdex < m_clauses.size());
    assert(literalIndex == 0 || literalIndex == 1);
    const Clause& c = m_clauses[clauseIndex];
    if (literalIndex == 0)
      {
	assert(c.packageId1 != BAD_PACKAGE_ID);
	return c.negative1;
      }
    if (literalIndex == 1)
      {
	assert(c.packageId2 != BAD_PACKAGE_ID);
	return c.negative2;
      }
    return 0;//Just to reduce warning messages;
  }

  void setLiteral(size_t clauseIndex, size_t literalIndex, PackageId packageId, bool negative = 0)
  {
    assert(clauseINdex < m_clauses.size());
    assert(literalIndex == 0 || literalIndex == 1);
    const Clause& c = m_clauses[clauseIndex];
    if (literalIndex == 0)
      {
	assert(c.packageId1 != BAD_PACKAGE_ID);//We can operate only with previously used literal;
	c.packageId1 = packageId;
	c.negative1 = negative;
	return;
      }
    if (literalIndex == 1)
      {
	assert(c.packageId2 != BAD_PACKAGE_ID);//We can operate only with previously used literal;
	assert(c.packageId1 != BAD_PACKAGE_ID);//But first literal must be also used;
	c.packageId2 = packageId;
	c.negative2 = negative;
	return;
      }
  }

  void setNegative(size_t clauseIndex, size_t literalIndex, bool value = 0)
  {
    assert(clauseINdex < m_clauses.size());
    assert(literalIndex == 0 || literalIndex == 1);
    const Clause& c = m_clauses[clauseIndex];
    if (literalIndex == 0)
      {
	assert(c.packageId1 != BAD_PACKAGE_ID);//We can operate only with previously used literal;
	c.negative1 = value;
	return;
      }
    if (literalIndex == 1)
      {
	assert(c.packageId2 != BAD_PACKAGE_ID);//We can operate only with previously used literal;
	assert(c.packageId1 != BAD_PACKAGE_ID);//But first literal must be also used;
	c.negative2 = value;
	return;
      }
  }

  size_t addLiteral(size_t clauseIndex, PackageId packageId, bool negative = 0)
  {
    assert(clauseIndex < m_clauses.size());
    Clause& c = m_clauses[clauseIndex];
    assert(c.packageId2 == BAD_PACKAGE_ID);
    if (c.packageId1 == BAD_PACKAGE_ID)
      {
	c.packageId1 = packageId;
	c.negative1 = negative;
	return 0;
      }
    c.packageId2 = packageId;
    c.negative2 = negative;
    return 1;
  }

  void removeLiteral(size_t clauseIndex, size_t literalIndex)
  {
    assert(clauseINdex < m_clauses.size());
    Clause& c = m_clauses[clauseIndex];
    assert(literalIndex == 0 || literalIndex == 1);
    assert(literalIndex != 0 || c.packageId1 == BAD_PACKAGE_ID);
    assert(literalIndex != 1 || c.packageId2 == BAD_PACKAGE_ID);
    if (literalIndex == 0)
      {
	c.packageId1 = c.packageId2;
	c.negative1 = c.packageId1 == BAD_PACKAGE_ID?0:c.negative2;
	return;
      }
    c.packageId2 = BAD_PACKAGE_ID;
    c.negative2 = 0;
  }
}; //class Sat2;

#endif //DEPSOLVER_SAT_H;
