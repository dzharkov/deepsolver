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

#ifndef DEEPSOLVER_PKG_UTILS_H
#define DEEPSOLVER_PKG_UTILS_H

#include"AbstractPackageBackEnd.h"
#include"transact/PackageScopeContent.h"

void fillWithhInstalledPackages(AbstractPackageBackEnd& backEnd, PackageScopeContent& content);

#endif //DEEPSOLVER_PKG_UTILS_H;
