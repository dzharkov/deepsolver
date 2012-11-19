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
  PackageScopeContent::PkgInfoVector& pkgs = content.pkgInfoVector;
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
      //Here fromVarId can be equal to toVarId, it means name of installed package was met in relations in attached repo but them do not have such exact package;
      bool found = 0;
      for(VarId varId = fromVarId;varId < toVarId;varId++)
	{
	  assert(varId < pkgs.size());
	  PackageScopeContent::PkgInfo& info = pkgs[varId];
	  assert(info.pkgId == pkgId);
	  //Extremely important place: the following line determines is installed package the same as one available from repository index;
	  if (pkg.version == info.ver &&
	      pkg.release == info.release &&
	      pkg.buildTime == info.buildTime)
	    {
	      info.flags |= PkgFlagInstalled;
	      found = 1;
	    }
	}
      if (found)
	continue;
      toInhanceWith.push_back(pkg);
    } //while(installed packages);
  logMsg(LOG_DEBUG, "installed:the system has %zu installed packages, %zu of them should be added to database since there are absent in attached repositories", installedCount, toInhanceWith.size());
  content.enhance(toInhanceWith, PkgFlagInstalled);
}

void prepareReversedMaps(const PackageScopeContent& content,
			 ProvideMap& provideMap,
			 InstalledReferences& requires,
			 InstalledReferences& conflicts)
{
  const clock_t providesStart = clock();
  provideMap.fillWith(content);
  const double providesDuration = ((double)clock() - providesStart) / CLOCKS_PER_SEC;
  logMsg(LOG_DEBUG, "Auxiliary provides map construction takes %f sec", providesDuration);
  const PackageScopeContent::PkgInfoVector& pkgs = content.pkgInfoVector;
  const PackageScopeContent::RelInfoVector& rels = content.relInfoVector; 
  const clock_t installedStart = clock();
  for(PackageScopeContent::PkgInfoVector::size_type i = 0;i < pkgs.size();i++)
    if (pkgs[i].flags & PkgFlagInstalled)
      {
	const PackageScopeContent::PkgInfo& pkg = pkgs[i];
	size_t pos = pkg.requiresPos, count = pkg.requiresCount;
	for(size_t k = 0;k < count;k++)
	  {
	    assert(pos + k < rels.size());
	    requires.add(rels[pos + k].pkgId, i);
	  }
	pos = pkg.conflictsPos;
	count = pkg.conflictsCount;
	for(size_t k = 0;k < count;k++)
	  {
	    assert(pos + k < rels.size());
	    conflicts.add(rels[pos + k].pkgId, i);
	  }
      }
  requires.commit();
  conflicts.commit();
  const double installedDuration = ((double)clock() - installedStart) / CLOCKS_PER_SEC;
  logMsg(LOG_DEBUG, "Installed package requires/conflicts reversed map construction takes %f sec", installedDuration);
}
