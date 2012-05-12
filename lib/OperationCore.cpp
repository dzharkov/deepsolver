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

static void buildTemporaryIndexFileNames(StringToStringMap& files, const std::string& tmpDirName)
{
  //FIXME:
}

void OperationCore::fetchIndices(AbstractIndexFetchListener& listener,
				 const AbstractOperationContinueRequest& continueRequest)
{
  const ConfRoot& root = m_conf.root();
  logMsg(LOG_DEBUG, "PkgData updating begin: pkgdatadir=\'%s\', tmpdir=\'%s\'", root.dir.pkgData.c_str(), root.dir.tmpPkgDataFetch.c_str());
  //FIXME:file lock;
  RepositoryVector repo;
  for(ConfRepoVector::size_type i = 0;i < root.repo.size();i++)
    for(StringVector::size_type k = 0;k < root.repo[i].arch.size();k++)
      for(StringVector::size_type j = 0;j < root.repo[i].components.size();j++)
	{
	  const std::string& arch = root.repo[i].arch[k];
	  const std::string& component = root.repo[i].components[j];
	  logMsg(LOG_DEBUG, "Registering repo \'%s, %s, %s\' for index update (%s)", root.repo[i].name.c_str(), root.repo[i].url.c_str(), arch.c_str(), component.c_str());
	  repo.push_back(Repository(root.repo[i], arch/, component));
	}
  StringToStringMap files;
  for(RepositoryVector::size_type i = 0;i < repo.size();i++)
    {
      repo[i].fetchInfoAndChecksum();
      repo[i].addIndexFilesForFetch(files);
    }
  logMsg(LOG_DEBUG, "List of index files to download consists of %zu entries", files.size());
  buildTemporaryIndexFileNames(files, "/tmp");//FIXME:directory name from ConfigCenter;
  IndexFetch indexFetch(listener, continueRequest);
  indexFetch.fetch(files);
  PackageScopeContentBuilder scope;
  for(RepositoryVector::size_type i = 0; i < repo.size();i++)
    repo[i].addPackagesToScope(files, scope);
  //FIXME:saving;
  //FIXME:removing old and renaming;
  //FIXME:temporary directory clean up;
  //FIXME:remove file lock;
}
