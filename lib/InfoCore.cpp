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
#include"InfoCore.h"
#include"io/PackageScopeContentLoader.h"

void InfoCore::availablePackages(PkgVector& pkgs)
{
  PackageScopeContent content;
  PackageScopeContentLoader loader(content);
  loader.loadFromFile(Directory::mixNameComponents(m_conf.root().dir.pkgData, PKG_DATA_FILE_NAME));
  const PackageScopeContent::PkgInfoVector& p = content.pkgInfoVector;
  pkgs.resize(p.size());
  for(PackageScopeContent::PkgInfoVector::size_type i = 0;i < p.size();i++)
    {
      pkgs[i].name = content.packageIdToStr(p[i].pkgId);
      pkgs[i].epoch = p[i].epoch;
      pkgs[i].version = p[i].ver;
      pkgs[i].release = p[i].release;
    }
}
