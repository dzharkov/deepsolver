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
#include"PkgUtils.h"

void fillWithhInstalledPackages(AbstractPackageBackEnd& backEnd, PackageScopeContent& content)
{
  logMsg(LOG_DEBUG, "Filling package scope content with installed packages");
  const PackageScopeContent::PkgInfoVector& pkgs = content.pkgInfoVector;
  std::auto_ptr<AbstractInstalledPackagesIterator> it = backEnd.enumInstalledPackages();
  size_t installedCount = 0;
  PkgVector toInhanceWith;
  Pkg pkg;
  while(it->moveNext(pkg))
    {
      installedCount++;
      if (!content.checkName(pkg.name))
	{
	  toInhanceWith.push_back(pkg);
	  continue;
	}
      const PackageId pkgId = content.strToPackageId(pkg.name);//FIXME:must be got with checkName();
      VarId fromVarId, toVarId;
      content.locateRange(pkgId, fromVarId, toVarId);
      if (fromVarId == toVarId)
	{
	  //We have pkgId but have no corresponding varId, it is slightly strange but actually not a problem;
	  logMsg(LOG_WARNING, "Package \'%s\' with corresponding pkgId=%zu has no any varId", pkg.name.c_str(), pkgId);
	}
      VarId matchingVarId = BAD_VAR_ID;
      for(VarId varId = fromVarId;varId < toVarId;varId++)
	{
	  assert(varId < pkgs.size());
	  const PackageScopeContent::PkgInfo& info = pkgs[varId];
	  assert(info.pkgId == pkgId);
	  //Extremely important place: the following line determines is installed package the same as one available from repository index;
	  if (pkg.version == info.ver && pkg.release == info.release && pkg.buildTime == info.buildTime)
	    {
	      matchingVarId = varId;
	      break;
	    }
	}
      if (matchingVarId != BAD_VAR_ID)
	continue;
      toInhanceWith.push_back(pkg);
    } //while(installed packages);
  logMsg(LOG_DEBUG, "The system has %zu installed packages, %zu of them should be added to database since there are absent in attached repositories", installedCount, toInhanceWith.size());
  content.enhance(toInhanceWith);
  logMsg(LOG_DEBUG, "The database of known packages was updated with list of installed packages");
}
