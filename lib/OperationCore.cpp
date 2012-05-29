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
#include"IndexFetch.h"
#include"AbstractPackageBackEnd.h"
#include"PackageScopeContentLoader.h"
#include"transact/PackageScope.h"

static void addINstalledPackageToScope(PackageScope& scope, AbstractInstalledPackagesIterator& it, Pkg& pkg)
{
  //FIXME:
}

static void fillWithhInstalledPackages(AbstractPackageBackEnd& backEnd, PackageScope& scope, PackageScopeContent& content)
{
  const PackageScopeContent::PkgInfoVector& pkgs = content.getPkgs();
  std::auto_ptr<AbstractInstalledPackagesIterator> it = backEnd.enumInstalledPackages();
  Pkg pkg;
  while(it->moveNext(pkg))
    {
      if (!content.checkName(pkg.name))
	{
	  addINstalledPackageToScope(scope, *it.get(), pkg);
	  continue;
	}
      const PackageId pkgId = content.strToPackageId(pkg.name);//FIXME:must be got with checkName();
      VarId fromVarId, toVarId;
      PackageScopeContent::locateRange(pkgs, pkgId, fromVarId, toVarId);
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
	  if (pkg.version == info.ver && pkg.release == info.release)
	    {
	      matchingVarId = varId;
	      break;
	    }
	}
      if (matchingVarId != BAD_VAR_ID)
	continue;
      //FIXME:
    } //while();
}

static std::string urlToFileName(const std::string& url)
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

static void buildTemporaryIndexFileNames(StringToStringMap& files, const std::string& tmpDirName)
{
  for(StringToStringMap::iterator it = files.begin();it != files.end();it++)
    it->second = Directory::mixNameComponents(tmpDirName, urlToFileName(it->first));
}

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
  for(RepositoryVector::size_type i = 0; i < repo.size();i++)
    repo[i].addPackagesToScope(files, scope);
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
  std::auto_ptr<AbstractPackageBackEnd> backEnd = createRpmBackEnd();
  PackageScopeContentLoader content;
  content.loadFromFile(Directory::mixNameComponents(m_conf.root().dir.pkgData, PKG_DATA_FILE_NAME));
  PackageScope scope(scope);
  fillWithhInstalledPackages(*backEnd.get(), scope, content);
}
