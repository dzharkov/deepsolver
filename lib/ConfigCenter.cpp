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

  static std::string stringBung;
static std::string buildConfigParamTitle(const StringVector& path, const std::string& sectArg);
//    throw ConfigException(ConfigErrorValueCannotBeEmpty, "core.dir.pkgdata", pos);


void ConfigCenter::initValues()
{
}

void ConfigCenter::loadFromFile(const std::string& fileName)
{
  logMsg(LOG_DEBUG, "Reading configuration data from \'%s\'", fileName.c_str());
  assert(!fileName.empty());
  File f;
  f.openReadOnly(fileName);
  StringVector lines;
  f.readTextFile(lines);
  f.close();
  ConfigFile parser(*this, fileName);
  for(StringVector::size_type i = 0;i < lines.size();i++)
      parser.processLine(lines[i]);
}

void ConfigCenter::commit()
{
  logMsg(LOG_DEBUG, "Committing loaded configuration data");
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    if (!m_stringValues[i].canBeEmpty && m_stringValues[i].value->empty())
      throw ConfigException(ConfigErrorValueCannotBeEmpty, m_stringValues[i].pathToString());
  m_root.dir.tmpPkgDataFetch = Directory::mixNameComponents(m_root.dir.pkgData, PKG_DATA_FETCH_DIR);//Real constant can be found in DefaultValues.h;
}

void ConfigCenter::onConfigFileValue(const StringVector& path, 
		       const std::string& sectArg,
		       const std::string& value,
				     bool adding,
				     const ConfigFilePosInfo& pos)
{
  assert(!path.empty());
  const int paramType = getParamType(path, sectArg, pos);
  if (paramType == ValueTypeString)
    {
      processStringValue(path, sectArg, value, adding, pos);
      return;
    }
  assert(0);
}

void ConfigCenter::processStringValue(const StringVector& path, 
				      const std::string& sectArg,
				      const std::string& value,
				      bool adding,
				      const ConfigFilePosInfo& pos)
{
  StringValue stringValue(stringBung);
  findStringValue(path, sectArg, stringValue);
}

ConfRepo& ConfigCenter::findRepo(const std::string& name)
{
  for(ConfRepoVector::size_type i = 0;i < m_root.repo.size();i++)
    if (m_root.repo[i].name == name)
      return m_root.repo[i];
  m_root.repo.push_back(ConfRepo(name));
  return m_root.repo.back();
}

int ConfigCenter::getParamType(const StringVector& path, const std::string& sectArg, const ConfigFilePosInfo& pos) const
{
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    if (m_stringValues[i].pathMatches(path, sectArg))
      return ValueTypeString;
  throw ConfigException(ConfigErrorUnknownParam, buildConfigParamTitle(path, sectArg), pos);
}

void ConfigCenter::findStringValue(const StringVector& path, 
				   const std::string& sectArg,
				   StringValue& stringValue)
{
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    if (m_stringValues[i].pathMatches(path, sectArg))
      stringValue = m_stringValues[i];
  assert(0);
}

std::string buildConfigParamTitle(const StringVector& path, const std::string& sectArg)
{
  assert(!path.empty());
  std::string value = path[0];
  if (!sectArg.empty())
    value += " \"" + sectArg + "\"";
  for(StringVector::size_type i = 1;i < path.size();i++)
    value += "." + path[i];
  return value;
}
