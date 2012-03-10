/*
   Copyright 2011-2012 ALT Linux
   Copyright 2011-2012 Dmitry V. Levin
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

class Sat
{
public:
  virtual ~Sat() {}

public:
  virtual size_t getClauseCount() const = 0;
  virtual size_t getLiteralCount(size_t clauseIndex) const = 0;
  virtual bool empty() const = 0;
  virtual void clear() = 0;
  virtual size_t addEmptyClause() = 0;
  virtual void removeClause(size_t clauseIndex) = 0;
  virtual PackageId getPackageId(size_t clauseIndex, size_t literalIndex) const = 0;
  virtual bool isNegative(size_t clauseIndex, size_t literalIndex) const = 0;
  virtual void setLiteral(size_t clauseIndex, size_t literalIndex, PackageId packageId, bool negative = 0) = 0;
  virtual void setNegative(size_t clauseIndex, size_t literalIndex, bool value = 0) = 0;
  virtual size_t addLiteral(size_t clauseIndex, PackageId packageId, bool negative = 0) = 0;
  virtual void removeLiteral(size_t clauseIndex, size_t literalIndex) = 0;
}; //class Sat;

#endif //DEEPSOLVER_SAT_H;
