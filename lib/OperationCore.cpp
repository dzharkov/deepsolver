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
  //FIXME:file lock;
  RepositoryVector repo;
  for(ConfRepoVector::size_type i = 0;i < root.repo.size();i++)
    repo.push_back(Repository(root.repo[i]));
  StringToStringMap files;
  for(RepositoryVector::size_type i = 0;i < repo.size();i++)
    {
      repo[i].fetchInfoAndChecksum();
      repo[i].addIndexFilesForFetch(files);
    }
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
