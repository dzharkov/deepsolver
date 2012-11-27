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
  const clock_t fillingStart = clock();
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
  const double fillingDuration = ((double)clock() - fillingStart) / CLOCKS_PER_SEC;
  logMsg(LOG_DEBUG, "pkg-utils:installed packages adding takes %f sec", fillingDuration);
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

void printSat(const PackageScope& scope, 
	      const Sat& sat,
	      const StringVector& annotations)
{
  for(Sat::size_type i = 0;i < sat.size();i++)
    {
      const Clause& clause = sat[i];
      if (i < annotations.size())
	std::cout << annotations[i] << std::endl;
      std::cout << "(" << std::endl;
	for(Clause::size_type k = 0;k < clause.size();k++)
	  {
	    const Lit& lit = clause[k];
	    if (lit.neg)
	      std::cout << " !"; else 
	      std::cout << "  ";
	    std::cout << scope.constructPackageNameWithBuildTime(lit.varId);
	    if (k + 1 < clause.size())
	      std::cout << " ||";
	    std::cout << std::endl;
	  }
	std::cout << ")" << std::endl;
	if (i + 1 < sat.size())
	  {
	    std::cout << std::endl;
	    std::cout << "&&" << std::endl;
	    std::cout << std::endl;
	  }
    }
}

void printSolution(const PackageScope& scope,
		   const VarIdVector& install,
		   const VarIdVector& remove)
{
  std::cout << install.size() << " to install, " << remove.size() << " to remove" << std::endl;
  std::cout << std::endl;
  std::cout << "The following packages must be installed:" << std::endl;
  for(size_t k = 0;k < install.size();k++)
    std::cout << scope.constructPackageName(install[k]) << std::endl;
  std::cout << std::endl;
  std::cout << "The following packages must be removed:" << std::endl;
  for(size_t k = 0;k < remove.size();k++)
    std::cout << scope.constructPackageName(remove[k]) << std::endl;
  std::cout << std::endl;
}
