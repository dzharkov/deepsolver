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
#include"ConfigCenter.h"

static std::string buildConfigParamTitle(const StringVector& path, const std::string& sectArg)
{
  assert(!path.empty());
  std::string value = path[0];
  if (!sectArg.empty())
    value += " \"" + sectArg + "\"";
  for(StringVector::size_type i = 1;i < path.size();i++)
    value += "." + path[i];
  return value;
}

void ConfigCenter::loadFromFile(const std::string& fileName)
{
  logMsg(LOG_DEBUG, "Reading configuration data from \'%s\'", fileName.c_str());
  assert(!fileName.empty());
  std::ifstream ifile(fileName.c_str());
  assert(ifile);//FIXME:exception;
  ConfigFile parser(*this, fileName);
  std::string line;
  while(1)
    {
      std::getline(ifile, line);
      if (!ifile)
	break;
      parser.processLine(line);
    }
}

void ConfigCenter::commit()
{
  logMsg(LOG_DEBUG, "Committing loaded configuration data");
  if (m_root.dir.pkgData.empty())
    throw ConfigException(ConfigErrorValueCannotBeEmpty, "core.dir.pkgdata");
  m_root.dir.tmpPkgDataFetch = Directory::mixNameComponents(m_root.dir.pkgData, PKG_DATA_FETCH_DIR);//Real constant can be found in DefaultValues.h;
  //Repositories;
  for(ConfRepoVector::size_type i = 0;i < m_root.repo.size();i++)
    {
      const ConfRepo& repo = m_root.repo[i];
      assert(!repo.name.empty());
      if (repo.url.empty())
	throw ConfigException(ConfigErrorValueCannotBeEmpty, "repo \"" + repo.name + "\".url");
      if (repo.arch.empty())
	throw ConfigException(ConfigErrorValueCannotBeEmpty, "repo \"" + repo.name + "\".arch");
      if (repo.components.empty())
	throw ConfigException(ConfigErrorValueCannotBeEmpty, "repo \"" + repo.name + "\".components");
      for(StringVector::size_type k = 0;k < repo.arch.size();k++)
	{
	  assert(!trim(repo.arch[k]).empty());
	}
      for(StringVector::size_type k = 0;k < repo.components.size();k++)
	{
	  assert(!trim(repo.components[k]).empty());
	}
    }
}

void ConfigCenter::onConfigFileValue(const StringVector& path, 
		       const std::string& sectArg,
		       const std::string& value,
				     bool adding,
				     const ConfigFilePosInfo& pos)
{
  assert(!path.empty());
  if (path[0] == "core")
    onCoreConfigValue(path, sectArg, value,adding, pos); else
  if (path[0] == "repo")
    onRepoConfigValue(path, sectArg, value,adding, pos); else
    throw ConfigException(ConfigErrorUnknownParam, buildConfigParamTitle(path, sectArg), pos);
}

void ConfigCenter::onCoreConfigValue(const StringVector&path,
				     const std::string& sectArg,
				     const std::string& value,
				     bool adding, 
				     const ConfigFilePosInfo& pos)
{
  //FIXME:sectArg must be empty;
  assert(sectArg.empty());
  if (path.size() < 2)
    throw ConfigException(ConfigErrorIncompletePath, buildConfigParamTitle(path, sectArg), pos);
  if (path[1] == "dir")
    onCoreDirConfigValue(path, sectArg, value,adding, pos); else
  throw ConfigException(ConfigErrorUnknownParam, buildConfigParamTitle(path, sectArg), pos);
}

void ConfigCenter::onCoreDirConfigValue(const StringVector&path,
				     const std::string& sectArg,
				     const std::string& value,
				     bool adding, 
				     const ConfigFilePosInfo& pos)
{
  //FIXME:sectArg must be empty;
  assert(sectArg.empty());
  if (path.size() < 3)
    throw ConfigException(ConfigErrorIncompletePath, buildConfigParamTitle(path, sectArg), pos);
  if (path[2] == "pkgdata")
    {
      if (adding)
    throw ConfigException(ConfigErrorAddingNotPermitted, buildConfigParamTitle(path, sectArg), pos);
      m_root.dir.pkgData = trim(value);
      if (m_root.dir.pkgData.empty())
    throw ConfigException(ConfigErrorValueCannotBeEmpty, "core.dir.pkgdata", pos);
      return;
    }
  throw ConfigException(ConfigErrorUnknownParam, buildConfigParamTitle(path, sectArg), pos);
}

void ConfigCenter::onRepoConfigValue(const StringVector&path,
				     const std::string& sectArg,
				     const std::string& value,
				     bool adding, 
				     const ConfigFilePosInfo& pos)
{
  if (path.size() < 2)
    throw ConfigException(ConfigErrorIncompletePath, buildConfigParamTitle(path, sectArg), pos);
  //URL;
  if (path[1] == "url")
    {
      if (adding)
    throw ConfigException(ConfigErrorAddingNotPermitted, buildConfigParamTitle(path, sectArg), pos);
      if (trim(value).empty())
	throw ConfigException(ConfigErrorValueCannotBeEmpty, buildConfigParamTitle(path, sectArg), pos);
      if (sectArg.empty())
	{
	  for(ConfRepoVector::size_type i = 0;i < m_root.repo.size();i++)
	    m_root.repo[i].url = trim(value);
	} else
	findRepo(sectArg).url = trim(value);
      return;
    } //URL;
  //Arch;
  if (path[1] == "arch")
    {
      StringVector values;
      splitBySpaces(value, values);
      if (sectArg.empty())
	{
	  for(ConfRepoVector::size_type i = 0;i < m_root.repo.size();i++)
	    {
	      if (!adding)
		m_root.repo[i].arch = values; else 
		for(StringVector::size_type k = 0;k < values.size();k++)
		  m_root.repo[i].arch.push_back(values[k]);
	    } //for(repos);
	} else
	{
	  ConfRepo& repo = findRepo(sectArg);
	  if (!adding)
	    repo.arch = values; else
	    for(StringVector::size_type k = 0;k < values.size();k++)
	      repo.arch.push_back(values[k]);
	}
      return;
    } //arch;


  //Components;
  if (path[1] == "components")
    {
      StringVector values;
      splitBySpaces(value, values);
      if (sectArg.empty())
	{
	  for(ConfRepoVector::size_type i = 0;i < m_root.repo.size();i++)
	    {
	      if (!adding)
		m_root.repo[i].components = values; else 
		for(StringVector::size_type k = 0;k < values.size();k++)
		  m_root.repo[i].components.push_back(values[k]);
	    } //for(repos);
	} else
	{
	  ConfRepo& repo = findRepo(sectArg);
	  if (!adding)
	    repo.components = values; else
	    for(StringVector::size_type k = 0;k < values.size();k++)
	      repo.components.push_back(values[k]);
	}
      return;
    } //Components;
  //FIXME:enabled;
  //FIXME:vendor;
  throw ConfigException(ConfigErrorUnknownParam, buildConfigParamTitle(path, sectArg), pos);
}

ConfRepo& ConfigCenter::findRepo(const std::string& name)
{
  for(ConfRepoVector::size_type i = 0;i < m_root.repo.size();i++)
    if (m_root.repo[i].name == name)
      return m_root.repo[i];
  m_root.repo.push_back(ConfRepo(name));
  return m_root.repo.back();
}
