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
#include"AbstractPackageBackEnd.h"

class RpmInstalledPackagesIterator: public AbstractInstalledPackagesIterator
{
public:
  RpmInstalledPackagesIterator() {}
  virtual ~RpmInstalledPackagesIterator() {}

public:
  bool moveNext(Pkg& pkg)
  {
    //FIXME:
  }

  void fillFullInfo(Pkg& pkg)
  {
    //FIXME:
  }
}; //class RpmInstalledPackagesIterator;

class RpmBackEnd: public AbstractPackageBackEnd
{
public:
  RpmBackEnd() {}
  virtual ~RpmBackEnd() {}

public:
  std::auto_ptr<AbstractInstalledPackagesIterator> enumInstalledPackages()
  {
  }
}; //class RpmBackEnd;

std::auto_ptr<AbstractPackageBackEnd> createRpmBackEnd()
{
  return std::auto_ptr<AbstractPackageBackEnd>(new RpmBackEnd());
}
