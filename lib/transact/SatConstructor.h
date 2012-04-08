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

#ifndef DEEPSOLVER_SAT_CONSTRUCTOR_H
#define DEEPSOLVER_SAT_CONSTRUCTOR_H

#include"PackageScope.h"
#include"sat/Sat.h"

class SatConstructor
{
public:
  SatConstructor(const PackageScope& scope)
    : m_scope(scope) {}

  ~SatConstructor() {}

public:
  void construct(const VarIdVector& strongToInstall,
		 const VarIdVector& strongToRemove,
		 Sat& sat) const;

private:
  const PackageScope& m_scope;
  VarIdSet m_pendingToInstall, m_pendingToRemove;
}; //class SatConstructor;

#endif //DEEPSOLVER_SAT_CONSTRUCTOR_H;
