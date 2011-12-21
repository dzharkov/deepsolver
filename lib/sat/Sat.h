
#ifndef DEPSOLVER_SAT_H
#define DEPSOLVER_SAT_H

class Sat
{
public:
  virtual ~Sat() {}

public:
  virtual size_t getClauseCount() const = 0;
  virtual size_t getLiteralCount(size_t clauseIndex) const = 0;
  virtual size_t addEmptyClause() = 0;
  virtual void removeClause(size_t clauseIndex) = 0;
  virtual PackageId getPackageId(size_t clauseIndex, size_t literalIndex) const = 0;
  virtual bool isNegative(size_t clauseIndex, size_t literalIndex) const = 0;
  virtual void setLiteral(size_t clauseIndex, size_t literalIndex, PackageId packageId, bool negative = 0) = 0;
  virtual void setNegative(size_t clauseIndex, size_t literalIndex, bool value = 0) = 0;
  virtual size_t addLiteral(size_t clauseIndex, PackageId packageId, bool negative = 0) = 0;
  virtual void removeLiteral(size_t clauseIndex, size_t literalIndex) = 0;
}; //class Sat;

#endif //DEPSOLVER_SAT_H;
