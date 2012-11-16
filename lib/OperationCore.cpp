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
#include"OperationCore.h"
#include"Repository.h"
#include"PackageInfoProcessor.h"
#include"IndexFetch.h"
#include"AbstractPackageBackEnd.h"
#include"AbstractTaskSolver.h"
#include"io/PackageScopeContentLoader.h"
#include"io/PackageScopeContentBuilder.h"
#include"PkgUtils.h"

static void printSat(const PackageScope& scope,
		     const Sat& sat,
		     const StringVector& annotations);

static void printSolution(const PackageScope& scope,
			  const VarIdVector& install,
			  const VarIdVector& remove);

static std::string urlToFileName(const std::string& url);
static void buildTemporaryIndexFileNames(StringToStringMap& files, const std::string& tmpDirName);

void OperationCore::fetchIndices(AbstractIndexFetchListener& listener,
				 const AbstractOperationContinueRequest& continueRequest)
{
  const ConfRoot& root = m_conf.root();
  logMsg(LOG_DEBUG, "PkgData updating begin: pkgdatadir=\'%s\', tmpdir=\'%s\'", root.dir.pkgData.c_str(), root.dir.tmpPkgDataFetch.c_str());
  listener.onInfoFilesFetch();
  //FIXME:file lock;
  RepositoryVector repo;
  for(ConfRepoVector::size_type i = 0;i < root.repo.size();i++)
    for(StringVector::size_type k = 0;k < root.repo[i].arch.size();k++)
      for(StringVector::size_type j = 0;j < root.repo[i].components.size();j++)
	{
	  const std::string& arch = root.repo[i].arch[k];
	  const std::string& component = root.repo[i].components[j];
	  logMsg(LOG_DEBUG, "Registering repo \'%s\' for index update (\'%s\', %s, %s)", root.repo[i].name.c_str(), root.repo[i].url.c_str(), arch.c_str(), component.c_str());
	  repo.push_back(Repository(root.repo[i], arch, component));
	}
  StringToStringMap files;
  for(RepositoryVector::size_type i = 0;i < repo.size();i++)
    {
      repo[i].fetchInfoAndChecksum();
      repo[i].addIndexFilesForFetch(files);
    }
  buildTemporaryIndexFileNames(files, root.dir.tmpPkgDataFetch);
  logMsg(LOG_DEBUG, "List of index files to download consists of %zu entries:", files.size());
  for(StringToStringMap::const_iterator it = files.begin();it != files.end();it++)
    logMsg(LOG_DEBUG, "Download entry: \'%s\' -> \'%s\'", it->first.c_str(), it->second.c_str());
  listener.onIndexFetchBegin();
  if (Directory::isExist(root.dir.tmpPkgDataFetch))
    logMsg(LOG_WARNING, "Directory \'%s\' already exists, probably unfinished previous transaction", root.dir.tmpPkgDataFetch.c_str());
  logMsg(LOG_DEBUG, "Preparing directory \'%s\', it must exist and be empty", root.dir.tmpPkgDataFetch.c_str());
  Directory::ensureExistsAndEmpty(root.dir.tmpPkgDataFetch, 1);//1 means erase any content;
  IndexFetch indexFetch(listener, continueRequest);
  indexFetch.fetch(files);
  listener.onIndexFilesReading();
  PackageScopeContentBuilder scope;
  PackageInfoProcessor infoProcessor;
  for(RepositoryVector::size_type i = 0; i < repo.size();i++)
    repo[i].loadPackageData(files, scope, infoProcessor);
  logMsg(LOG_DEBUG, "Committing loaded packages data");
  scope.commit();
  const std::string outputFileName = Directory::mixNameComponents(root.dir.pkgData, PKG_DATA_FILE_NAME);
  logMsg(LOG_DEBUG, "Saving constructed package data to \'%s\'", outputFileName.c_str());
  scope.saveToFile(outputFileName);
  //FIXME:The current code is working but it should create temporary file elsewhere and then replace with it already existing outputFileName;
  logMsg(LOG_DEBUG, "Clearing and removing \'%s\'", root.dir.tmpPkgDataFetch.c_str());
  Directory::eraseContent(root.dir.tmpPkgDataFetch);
  Directory::remove(root.dir.tmpPkgDataFetch);
  logMsg(LOG_INFO, "Repository index updating finished!");
  //FIXME:remove file lock;
  listener.onIndexFetchComplete();
}

void OperationCore::doInstallRemove(const UserTask& userTask)
{
  File::readAhead("/var/lib/rpm/Packages");//FIXME:take the value from configuration;
  std::auto_ptr<AbstractPackageBackEnd> backEnd = CREATE_PACKAGE_BACKEND;
  PackageScopeContent content;
  PackageScopeContentLoader loader(content);
  loader.loadFromFile(Directory::mixNameComponents(m_conf.root().dir.pkgData, PKG_DATA_FILE_NAME));
  logMsg(LOG_DEBUG, "Index package list loaded");
  const clock_t fillingStart = clock();
  fillWithhInstalledPackages(*backEnd.get(), content);
  const double fillingDuration = ((double)clock() - fillingStart) / CLOCKS_PER_SEC;
  logMsg(LOG_DEBUG, "Installed packages adding takes %f sec", fillingDuration);
  logMsg(LOG_DEBUG, "Merged list of installed packages");
  ProvideMap provideMap;
  InstalledReferences requiresReferences, conflictsReferences;
  provideMap.fillWith(content);
  logMsg(LOG_DEBUG, "Provide map filled");
  const PackageScopeContent::PkgInfoVector& pkgs = content.pkgInfoVector;
  const PackageScopeContent::RelInfoVector& rels = content.relInfoVector; 
  for(PackageScopeContent::PkgInfoVector::size_type i = 0;i < pkgs.size();i++)
    if (pkgs[i].flags & PkgFlagInstalled)
      {
	const PackageScopeContent::PkgInfo& pkg = pkgs[i];
	size_t pos = pkg.requiresPos, count = pkg.requiresCount;
	for(size_t k = 0;k < count;k++)
	  {
	    assert(pos + k < rels.size());
	    requiresReferences.add(rels[pos + k].pkgId, i);
	  }
	pos = pkg.conflictsPos;
	count = pkg.conflictsCount;
	for(size_t k = 0;k < count;k++)
	  {
	    assert(pos + k < rels.size());
	    conflictsReferences.add(rels[pos + k].pkgId, i);
	  }
      }
  requiresReferences.commit();
  conflictsReferences.commit();
  logMsg(LOG_DEBUG, "Requires and Conflicts references filled");
  std::auto_ptr<AbstractTaskSolver> solver = createGeneralTaskSolver(content, provideMap, requiresReferences, conflictsReferences);
  VarIdVector toInstall, toRemove;
  VarIdToVarIdMap toUpgrade;

  UserTask t;
  t.itemsToInstall.push_back(UserTaskItemToInstall("gnome3-default"));
  //  t.namesToRemove.insert("voiceman");
  //  t.namesToRemove.insert("gcc4.3");
  //  t.namesToRemove.insert("dbus");

  const clock_t solverStart = clock();
  solver->solve(t, toInstall, toRemove, toUpgrade);//FIXME:userTask;
  const double solverDuration = ((double)clock() - solverStart) / CLOCKS_PER_SEC;
  logMsg(LOG_DEBUG, "Solver takes %f seconds", solverDuration);

}

// Static functions;

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

std::string urlToFileName(const std::string& url)
{
  std::string s;
  for(std::string::size_type i = 0;i < url.length();i++)
    {
      const char c = url[i];
      if ((c >= 'a' && c <= 'z') ||
	  (c >= 'A' && c <= 'Z') ||
	  (c >= '0' && c<= '9') ||
	  c == '-' ||
	  c == '_')
	s += c; else 
	if (s.empty() || s[s.length() - 1] != '-')
	  s += '-';
    } //for();
  return s;
}

void buildTemporaryIndexFileNames(StringToStringMap& files, const std::string& tmpDirName)
{
  for(StringToStringMap::iterator it = files.begin();it != files.end();it++)
    it->second = Directory::mixNameComponents(tmpDirName, urlToFileName(it->first));
}
