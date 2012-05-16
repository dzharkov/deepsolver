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

#ifndef DEEPSOLVER_ABSTRACT_PACKAGE_BACK_END_H
#define DEEPSOLVER_ABSTRACT_PACKAGE_BACK_END_H

#include"Pkg.h"

class AbstractInstalledPackagesIterator
{
public:
  AbstractInstalledPackagesIterator() {}
  virtual ~AbstractInstalledPackagesIterator() {}

public:
  virtual bool moveNext(Pkg& pkg) = 0; 
  virtual void fillFullInfo(Pkg& pkg) = 0;
}; //class AbstractInstalledPackagesIterator;

class AbstractPackageBackEnd
{
public:
  AbstractPackageBackEnd() {}
  virtual ~AbstractPackageBackEnd() {}

public:
  virtual std::auto_ptr<AbstractInstalledPackagesIterator> enumInstalledPackages() = 0;
}; //class AbstractPackageBackEnd;

std::auto_ptr<AbstractPackageBackEnd> createRpmBackEnd();

#endif //DEEPSOLVER_ABSTRACT_PACKAGE_BACK_END_H;
