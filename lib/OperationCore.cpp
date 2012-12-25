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
#include"transact/AbstractTaskSolver.h"
#include"transact/AbstractSatSolver.h"
#include"transact/PackageScope.h"
#include"io/PackageScopeContentLoader.h"
#include"io/PackageScopeContentBuilder.h"
#include"PkgUtils.h"

static std::string urlToFileName(const std::string& url);
static void buildTemporaryIndexFileNames(StringToStringMap& files, const std::string& tmpDirName);

void OperationCore::fetchIndices(AbstractIndexFetchListener& listener,
				 const AbstractOperationContinueRequest& continueRequest) const
{
  const ConfRoot& root = m_conf.root();
  const std::string tmpDir = Directory::mixNameComponents(root.dir.pkgData, PKG_DATA_FETCH_DIR);
  logMsg(LOG_DEBUG, "operation:package data updating begin: pkgdatadir=\'%s\', tmpdir=\'%s\'", root.dir.pkgData.c_str(), tmpDir.c_str());
  listener.onInfoFilesFetch();
  //FIXME:file lock;
  RepositoryVector repo;
  for(ConfRepoVector::size_type i = 0;i < root.repo.size();i++)
    {
      if (!root.repo[i].enabled)
	continue;
      for(StringVector::size_type k = 0;k < root.repo[i].arch.size();k++)
	for(StringVector::size_type j = 0;j < root.repo[i].components.size();j++)
	  {
	    const std::string& arch = root.repo[i].arch[k];
	    const std::string& component = root.repo[i].components[j];
	    logMsg(LOG_DEBUG, "operation:registering repo \'%s\' for index update (\'%s\', %s, %s)", root.repo[i].name.c_str(), root.repo[i].url.c_str(), arch.c_str(), component.c_str());
	    repo.push_back(Repository(root.repo[i], arch, component));
	  }
    }
  StringToStringMap files;
  for(RepositoryVector::size_type i = 0;i < repo.size();i++)
    {
      repo[i].fetchInfoAndChecksum();
      repo[i].addIndexFilesForFetch(files);
    }
  buildTemporaryIndexFileNames(files, tmpDir);
  logMsg(LOG_DEBUG, "operation:list of index files to download consists of %zu entries:", files.size());
  for(StringToStringMap::const_iterator it = files.begin();it != files.end();it++)
    logMsg(LOG_DEBUG, "operation:download entry: \'%s\' -> \'%s\'", it->first.c_str(), it->second.c_str());
  listener.onIndexFetchBegin();
  if (Directory::isExist(tmpDir))
    logMsg(LOG_WARNING, "Directory \'%s\' already exists, probably unfinished previous transaction", tmpDir.c_str());
  logMsg(LOG_DEBUG, "operation:preparing directory \'%s\', it must exist and be empty", tmpDir.c_str());
  Directory::ensureExistsAndEmpty(tmpDir, 1);//1 means erase any content;
  if (!files.empty())
    {
      IndexFetch indexFetch(listener, continueRequest);
      indexFetch.fetch(files);
    }
  listener.onIndexFilesReading();
  PackageScopeContentBuilder scope;
  PackageInfoProcessor infoProcessor;
  for(RepositoryVector::size_type i = 0; i < repo.size();i++)
    repo[i].loadPackageData(files, scope, infoProcessor);
  logMsg(LOG_DEBUG, "operation:committing loaded packages data");
  scope.commit();
  const std::string outputFileName = Directory::mixNameComponents(root.dir.pkgData, PKG_DATA_FILE_NAME);
  logMsg(LOG_DEBUG, "operation:saving constructed package data to \'%s\'", outputFileName.c_str());
  scope.saveToFile(outputFileName);
  //FIXME:The current code is working but it should create temporary file elsewhere and then replace with it already existing outputFileName;
  logMsg(LOG_DEBUG, "operation:clearing and removing \'%s\'", tmpDir.c_str());
  Directory::eraseContent(tmpDir);
  Directory::remove(tmpDir);
  logMsg(LOG_INFO, "Repository index updating finished!");
  //FIXME:remove file lock;
  listener.onIndexFetchComplete();
}

void OperationCore::transaction(AbstractTransactionListener& listener, const UserTask& userTask) const
{
  const ConfRoot& root = m_conf.root();
  for(StringVector::size_type i = 0;i < root.os.transactReadAhead.size();i++)
    File::readAhead(root.os.transactReadAhead[i]);
  std::auto_ptr<AbstractPackageBackEnd> backEnd = CREATE_PACKAGE_BACKEND;
  backEnd->initialize();
  PackageScopeContent content;
  PackageScopeContentLoader loader(content);
  listener.onAvailablePkgListProcessing();
  loader.loadFromFile(Directory::mixNameComponents(m_conf.root().dir.pkgData, PKG_DATA_FILE_NAME));
  logMsg(LOG_DEBUG, "operation:index package list loaded");
  if (content.pkgInfoVector.empty())//FIXME:
    throw NotImplementedException("Empty set of attached repositories");
  listener.onInstalledPkgListProcessing();
  PkgUtils::fillWithhInstalledPackages(*backEnd.get(), content);
  ProvideMap provideMap;
  InstalledReferences requiresReferences, conflictsReferences;
  PkgUtils::prepareReversedMaps(content, provideMap, requiresReferences, conflictsReferences);
  listener.onInstallRemovePkgListProcessing();
  PackageScope scope(*backEnd.get(), content, provideMap, requiresReferences, conflictsReferences);
  TaskSolverData taskSolverData(*backEnd.get(), scope);
  for(ConfProvideVector::size_type i = 0;i < root.provide.size();i++)
    {
      assert(!trim(root.provide[i].name).empty());
      TaskSolverProvideInfo info(root.provide[i].name);
      for(StringVector::size_type k = 0;k < root.provide[i].providers.size();k++)
	info.providers.push_back(root.provide[i].providers[k]);
      taskSolverData.provides.push_back(info);
    }
  std::auto_ptr<AbstractTaskSolver> solver = createGeneralTaskSolver(taskSolverData);
  VarIdVector toInstall, toRemove;
  solver->solve(userTask, toInstall, toRemove);
  PkgUtils::printSolution(scope, toInstall, toRemove);
}

std::string OperationCore::generateSat(AbstractTransactionListener& listener, const UserTask& userTask) const
{
  const ConfRoot& root = m_conf.root();
  for(StringVector::size_type i = 0;i < root.os.transactReadAhead.size();i++)
    File::readAhead(root.os.transactReadAhead[i]);
  std::auto_ptr<AbstractPackageBackEnd> backEnd = CREATE_PACKAGE_BACKEND;
  backEnd->initialize();
  PackageScopeContent content;
  PackageScopeContentLoader loader(content);
  listener.onAvailablePkgListProcessing();
  loader.loadFromFile(Directory::mixNameComponents(m_conf.root().dir.pkgData, PKG_DATA_FILE_NAME));
  logMsg(LOG_DEBUG, "operation:index package list loaded");
  if (content.pkgInfoVector.empty())//FIXME:
    throw NotImplementedException("Empty set of attached repositories");
  listener.onInstalledPkgListProcessing();
  PkgUtils::fillWithhInstalledPackages(*backEnd.get(), content);
  ProvideMap provideMap;
  InstalledReferences requiresReferences, conflictsReferences;
  PkgUtils::prepareReversedMaps(content, provideMap, requiresReferences, conflictsReferences);
  PackageScope scope(*backEnd.get(), content, provideMap, requiresReferences, conflictsReferences);
  TaskSolverData taskSolverData(*backEnd.get(), scope);
  for(ConfProvideVector::size_type i = 0;i < root.provide.size();i++)
    {
      assert(!trim(root.provide[i].name).empty());
      TaskSolverProvideInfo info(root.provide[i].name);
      for(StringVector::size_type k = 0;k < root.provide[i].providers.size();k++)
	info.providers.push_back(root.provide[i].providers[k]);
      taskSolverData.provides.push_back(info);
    }
  std::auto_ptr<AbstractTaskSolver> solver = createGeneralTaskSolver(taskSolverData);
  return solver->constructSat(userTask);
}

// Static functions;

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
