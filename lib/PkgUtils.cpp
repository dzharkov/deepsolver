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

#include"deepsolver.h"
#include"PkgUtils.h"

static void printThreeColumns(const StringVector& items);

void PkgUtils::fillWithhInstalledPackages(AbstractPackageBackEnd& backEnd, PackageScopeContent& content)
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

void PkgUtils::prepareReversedMaps(const PackageScopeContent& content,
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

std::string PkgUtils::satToString(const AbstractPackageScope& scope, 
	      const Sat& sat,
	      const StringVector& annotations)
{
  std::ostringstream ss;
  for(Sat::size_type i = 0;i < sat.size();i++)
    {
      const Clause& clause = sat[i];
      if (i < annotations.size())
	ss << annotations[i] << std::endl;
      ss << "(" << std::endl;
	for(Clause::size_type k = 0;k < clause.size();k++)
	  {
	    const Lit& lit = clause[k];
	    if (lit.neg)
	      ss << " !"; else 
	      ss << "  ";
	    ss << scope.constructPackageNameWithBuildTime(lit.varId);
	    if (k + 1 < clause.size())
	      ss << " ||";
	    ss << std::endl;
	  }
	ss << ")" << std::endl;
	if (i + 1 < sat.size())
	  {
	    ss << std::endl;
	    ss << "&&" << std::endl;
	    ss << std::endl;
	  }
    }
  return ss.str();
}

void PkgUtils::printSolution(const AbstractPackageScope& scope,
			     const VarIdVector& install,
			     const VarIdVector& remove)
{
  std::cout << std::endl;
  std::cout << install.size() << " packages to install, " << remove.size() << " packages to remove" << std::endl;
  if (!install.empty())
    {
      std::cout << std::endl;
      std::cout << "The following packages must be installed:" << std::endl;
      StringVector v;
      for(size_t k = 0;k < install.size();k++)
	v.push_back(scope.constructPackageName(install[k]));
      printThreeColumns(v);
    }
  if (!remove.empty())
    {
      StringVector v;
      std::cout << std::endl;
      std::cout << "The following packages must be removed:" << std::endl;
      for(size_t k = 0;k < remove.size();k++)
	v.push_back(scope.constructPackageName(remove[k]));
      printThreeColumns(v);
      std::cout << std::endl;
    }
}

void printThreeColumns(const StringVector& items)
{
  StringVector v = items;
  std::sort(v.begin(), v.end());
  if (v.empty())
    return;
  StringVector v1, v2, v3;
  for(StringVector::size_type i = 0;i < v.size();i += 3)
    {
      v1.push_back(v[i]);
      if (i + 1 < v.size())
	v2.push_back(v[i + 1]);
      if (i + 2 < v.size())
	v3.push_back(v[i + 2]);
    }
  std::string::size_type maxLen1 = 0, maxLen2 = 0;
  for(StringVector::size_type i = 0;i < v1.size();i++)
    if (v1[i].length() > maxLen1)
      maxLen1 = v1[i].length();
  for(StringVector::size_type i = 0;i < v2.size();i++)
    if (v2[i].length() > maxLen2)
      maxLen2 = v2[i].length();
  assert(maxLen1 > 0 && (v2.empty() || maxLen2 > 0));
  for(StringVector::size_type i = 0;i < v3.size();i++)
    {
      std::cout << v1[i];
      for(std::string::size_type k = v1[i].length();k < maxLen1 + 2;k++)
	std::cout << " ";
      std::cout << v2[i];
      for(std::string::size_type k = v2[i].length();k < maxLen2 + 2;k++)
	std::cout << " ";
      std::cout << v3[i] << std::endl;
    }
  if (v1.size() > v3.size())
    {
      std::cout << v1[v1.size() - 1];
      if (v2.size() > v3.size())
	{
	  for(std::string::size_type k = v1[v1.size() - 1].length();k < maxLen1 + 2;k++)
	    std::cout << " ";
	  std::cout << v2[v2.size() - 1];
	}
      std::cout << std::endl;
    }
}
