/*
   Copyright 2011-2013 ALT Linux
   Copyright 2011-2013 Michael Pozhidaev

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

#ifndef DEEPSOLVER_PKG_UTILS_H
#define DEEPSOLVER_PKG_UTILS_H

#include"AbstractPackageBackEnd.h"
#include"transact/AbstractPackageScope.h"
#include"transact/AbstractSatSolver.h"
#include"transact/PackageScopeContent.h"
#include"transact/ProvideMap.h"
#include"transact/InstalledReferences.h"

class PkgUtils
{
public:
  static void fillWithhInstalledPackages(AbstractPackageBackEnd& backEnd, PackageScopeContent& content);

  static void prepareReversedMaps(const PackageScopeContent& content,
			   ProvideMap& provideMap,
			   InstalledReferences& requires,
			   InstalledReferences& conflicts);

  static std::string satToString(const AbstractPackageScope& scope, 
				 const Sat& sat,
				 const StringVector& annotations);

  static void printSolution(const AbstractPackageScope& scope,
			    const VarIdVector& install,
			    const VarIdVector& remove);
}; //class PkgUtils;

#endif //DEEPSOLVER_PKG_UTILS_H;
