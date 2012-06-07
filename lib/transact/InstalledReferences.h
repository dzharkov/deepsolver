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

#ifndef DEEPSOLVER_INSTALLED_REFERENCES_H
#define DEEPSOLVER_INSTALLED_REFERENCES_H

class InstalledReferences
{
public:
  InstalledReferences() {}
  virtual ~InstalledReferences() {}

public:
  InstalledReferences() {}
  virtual ~InstalledReferences() {}

public:
  void add(VarId refTo, VarId drefFrom);
  void commit();
  void searchReferencesTo(VarId refTo, VarIdVector& res);

private:
  struct reference
  {
    Reference()
      : refFrom(BAD_VAR_ID), refTo(BAD_VAR_ID) {}

    Reference(VarId t, VarId f)
      : refTo(t), refFrom(f) {}

    bool operator <(const reference& r) const
    {
      return refTo < r.refTo;
    }

    bool operator >(const Reference& r) const
    {
      return refTo > r.refTo;
    }

    VarId refTo, refFrom;
  }; //struct Reference;

  typedef std::vector<Reference> ReferenceVector;

private:
  ReferenceVector m_references;
}; //class InstalledReferences;

#endif //DEEPSOLVER_INSTALLED_REFERENCES_H;
