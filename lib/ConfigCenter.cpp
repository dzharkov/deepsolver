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
#include"utils/TextFiles.h"

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
  assert(!fileName.empty());
  std::auto_ptr<AbstractTextFileReader> file = createTextFileReader(TextFileStd, fileName);
  ConfigFile parser(*this, fileName);
  std::string line;
  while(file->readLine(line))
    parser.processLine(line);
}

void ConfigCenter::onConfigFileValue(const StringVector& path, 
		       const std::string& sectArg,
		       const std::string& value,
				     bool adding,
				     const ConfigFilePosInfo& pos)
{
  assert(!path.empty());
  if (path[0] == "repo")
    onRepoConfigValue(path, sectArg, value,adding, pos); else
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
  if (path[1] == "url")
    {
      //FIXME:no adding;
      if (sectArg.empty())
	{
	  for(ConfRepoVector::size_type i = 0;i < m_root.repo.size();i++)
	    m_root.repo[i].url = trim(value);
	} else
	findRepo(sectArg).url = trim(value);
      return;
    }
  //FIXME:enabled;
  //FIXME:components;
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
