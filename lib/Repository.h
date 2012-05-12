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

#ifndef DEEPSOLVER_REPOSITORY_H
#define DEEPSOLVER_REPOSITORY_H

#include"ConfigCenter.h"
#include"PackageScopeContentBuilder.h"

class Repository
{
public:
  Repository(const ConfRepo& confRepo,
	     const std:;string& arch,
	     const std::string& component)
    : m_url(confRepo.url), 
      m_arch(arch),
      m_component(component)
  {
    assert(!m_url.empty());
    assert(!m_arch.empty());
    assert(!m_component.empty());
  }

  virtual ~Repository() {}

public:
  std::string getUrl() const
  {
    assert(!m_url.empty());
    return m_url;
  }

  void fetchInfoAndChecksum();
  void addIndexFilesForFetch(StringToStringMap& files);
  void addPackagesToScope(const StringToStringMap& files, PackageScopeContentBuilder& content);

private:
  std::string buildInfoFileUrl() const;

private:
  std::string m_url, m_arch, m_component;

  std::string m_info;
  std::string m_checksum;
}; //class Repository;

typedef std::vector<Repository> RepositoryVector;
typedef std::list<Repository> RepositoryList;

#endif //DEEPSOLVER_REPOSITORY_H;
