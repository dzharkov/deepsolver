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

#define DELIMITERS ",;"

static std::string buildConfigParamTitle(const StringVector& path, const std::string& sectArg);
static bool parseBooleanValue(const StringVector& path,
		       const std::string& sectArg,
		       const std::string& str,
		       const ConfigFilePosInfo& pos);

void ConfigCenter::initValues()
{
  addNonEmptyStringParam3("core", "dir", "pkg-data", m_root.dir.pkgData );
}

void ConfigCenter::reinitRepoValues()
{
  m_repoStringValues.clear();
  m_repoStringListValues.clear();
  m_repoBooleanValues;
  for(ConfRepoVector::size_type i = 0;i < m_root.repo.size();i++)
    {
      ConfRepo& repo = m_root.repo[i];
      StringValue stringValue;
      stringValue.path.push_back("repo");
      stringValue.sectArg = repo.name;
      //URL;
      stringValue.path.push_back("url");
      stringValue.value = &repo.url;
      stringValue.canBeEmpty = 0;
      m_repoStringValues.push_back(stringValue);
      //Vendor;
      stringValue.path[1] = "vendor";
      stringValue.value = &repo.vendor;
      stringValue.canBeEmpty = 1;
      m_repoStringValues.push_back(stringValue);
      //Arch;
      StringListValue stringListValue;
      stringListValue.sectArg = repo.name;
      stringListValue.path.push_back("repo");
      stringListValue.path.push_back("arch");
      stringListValue.delimiters = DELIMITERS;
      stringListValue.canContainEmptyItem = 0;
      stringListValue.value = &repo.arch;
      m_repoStringListValues.push_back(stringListValue);
      //Components;
      stringListValue.path[1] = "components";
      stringListValue.value = &repo.components;
      m_repoStringListValues.push_back(stringListValue);
    }
}

void ConfigCenter::commit()
{
  logMsg(LOG_DEBUG, "Committing loaded configuration data");
  //General commit;
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    if (!m_stringValues[i].canBeEmpty && trim(*m_stringValues[i].value).empty())
      throw ConfigException(ConfigErrorValueCannotBeEmpty, m_stringValues[i].pathToString());
  for(StringValueVector::size_type i = 0;i < m_repoStringValues.size();i++)
    if (!m_stringValues[i].canBeEmpty && trim(*m_repoStringValues[i].value).empty())
      throw ConfigException(ConfigErrorValueCannotBeEmpty, m_stringValues[i].pathToString());
  //Custom commit;
  m_root.dir.tmpPkgDataFetch = Directory::mixNameComponents(m_root.dir.pkgData, PKG_DATA_FETCH_DIR);//Real constant can be found in DefaultValues.h;
}

void ConfigCenter::addStringParam3(const std::string& path1,
				   const std::string& path2,
				   const std::string& path3,
				   std::string& value)
{
  assert(!path1.empty() && !path2.empty() && !path3.empty());
  StringValue stringValue(value);
  stringValue.canBeEmpty = 1;
  stringValue.path.push_back(path1);
  stringValue.path.push_back(path2);
  stringValue.path.push_back(path3);
  m_stringValues.push_back(stringValue);
}

void ConfigCenter::addNonEmptyStringParam3(const std::string& path1,
					   const std::string& path2,
					   const std::string& path3,
					   std::string& value)
{
  assert(!path1.empty() && !path2.empty() && !path3.empty());
  StringValue stringValue(value);
  stringValue.canBeEmpty = 0;
  stringValue.path.push_back(path1);
  stringValue.path.push_back(path2);
  stringValue.path.push_back(path3);
  m_stringValues.push_back(stringValue);
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

void ConfigCenter::onConfigFileValue(const StringVector& path, 
		       const std::string& sectArg,
		       const std::string& value,
				     bool adding,
				     const ConfigFilePosInfo& pos)
{
  assert(!path.empty());
  if (path[0] == "repo")
    {
      assert(!sectArg.empty());//FIXME:
      ConfRepoVector::size_type i = 0;
      while(i < m_root.repo.size() && m_root.repo[i].name == sectArg)
	i++;
      if (i >= m_root.repo.size())
	m_root.repo.push_back(ConfRepo(sectArg));
    }
  const int paramType = getParamType(path, sectArg, pos);
  if (paramType == ValueTypeString)
    {
      processStringValue(path, sectArg, value, adding, pos);
      return;
    }
  if (paramType == ValueTypeStringList)
    {
      processStringListValue(path, sectArg, value, adding, pos);
      return;
    }
  if (paramType == ValueTypeBoolean)
    {
      processBooleanValue(path, sectArg, value, adding, pos);
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
  StringValue stringValue;
  findStringValue(path, sectArg, stringValue);
  assert(stringValue.value != NULL);
  if (!adding)
    (*stringValue.value) = value; else
    (*stringValue.value) += value;
}

void ConfigCenter::processStringListValue(const StringVector& path, 
				      const std::string& sectArg,
				      const std::string& value,
				      bool adding,
				      const ConfigFilePosInfo& pos)
{
  StringListValue stringListValue;
  findStringListValue(path, sectArg, stringListValue);
  assert(stringListValue.value != NULL);
  StringVector& v = *(stringListValue.value);
  if (adding)
    v.clear();
  std::string item;
  for(std::string::size_type i = 0;i < value.size();i++)
    {
      std::string::size_type p = 0;
      while(p < stringListValue.delimiters.size() && value[i] != stringListValue.delimiters[p])
	p++;
      if (p >= stringListValue.delimiters.size())
	{
	  item += value[i];
	  continue;
	}
      if (!stringListValue.canContainEmptyItem && trim(item).empty())
	throw ConfigException(ConfigErrorValueCannotBeEmpty, buildConfigParamTitle(path, sectArg), pos);
      v.push_back(item);
      item.erase();
    }
  if (!stringListValue.canContainEmptyItem && trim(item).empty())
    throw ConfigException(ConfigErrorValueCannotBeEmpty, buildConfigParamTitle(path, sectArg), pos);
  v.push_back(item);
}

void ConfigCenter::processBooleanValue(const StringVector& path, 
				      const std::string& sectArg,
				      const std::string& value,
				      bool adding,
				      const ConfigFilePosInfo& pos)
{
  BooleanValue booleanValue;
  findBooleanValue(path, sectArg, booleanValue);
  assert(booleanValue.value != NULL);
  bool& v = *(booleanValue.value);
  if (adding)
	throw ConfigException(ConfigErrorAddingNotPermitted, buildConfigParamTitle(path, sectArg), pos);
  v = parseBooleanValue(path, sectArg, value, pos);
}

int ConfigCenter::getParamType(const StringVector& path, const std::string& sectArg, const ConfigFilePosInfo& pos) const
{
  //String;
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    if (m_stringValues[i].pathMatches(path, sectArg))
      return ValueTypeString;
  for(StringValueVector::size_type i = 0;i < m_repoStringValues.size();i++)
    if (m_repoStringValues[i].pathMatches(path, sectArg))
      return ValueTypeString;
  //StringList;
  for(StringListValueVector::size_type i = 0;i < m_stringListValues.size();i++)
    if (m_stringListValues[i].pathMatches(path, sectArg))
      return ValueTypeStringList;
  for(StringListValueVector::size_type i = 0;i < m_repoStringListValues.size();i++)
    if (m_repoStringListValues[i].pathMatches(path, sectArg))
      return ValueTypeStringList;
  //Boolean;
  for(BooleanValueVector::size_type i = 0;i < m_booleanValues.size();i++)
    if (m_booleanValues[i].pathMatches(path, sectArg))
      return ValueTypeBoolean;
  for(BooleanValueVector::size_type i = 0;i < m_repoBooleanValues.size();i++)
    if (m_repoBooleanValues[i].pathMatches(path, sectArg))
      return ValueTypeBoolean;
  throw ConfigException(ConfigErrorUnknownParam, buildConfigParamTitle(path, sectArg), pos);
}

void ConfigCenter::findStringValue(const StringVector& path, 
				   const std::string& sectArg,
				   StringValue& stringValue)
{
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    if (m_stringValues[i].pathMatches(path, sectArg))
      {
	stringValue = m_stringValues[i];
	return;
      }
  for(StringValueVector::size_type i = 0;i < m_repoStringValues.size();i++)
    if (m_repoStringValues[i].pathMatches(path, sectArg))
      {
	stringValue = m_repoStringValues[i];
	return;
      }
  assert(0);
}

void ConfigCenter::findStringListValue(const StringVector& path, 
				   const std::string& sectArg,
				   StringListValue& stringListValue)
{
  for(StringListValueVector::size_type i = 0;i < m_stringListValues.size();i++)
    if (m_stringListValues[i].pathMatches(path, sectArg))
      {
	stringListValue = m_stringListValues[i];
	return;
      }
  for(StringListValueVector::size_type i = 0;i < m_repoStringValues.size();i++)
    if (m_repoStringListValues[i].pathMatches(path, sectArg))
      {
	stringListValue = m_repoStringListValues[i];
	return;
      }
  assert(0);
}

void ConfigCenter::findBooleanValue(const StringVector& path, 
				   const std::string& sectArg,
				   BooleanValue& booleanValue)
{
  for(BooleanValueVector::size_type i = 0;i < m_booleanValues.size();i++)
    if (m_booleanValues[i].pathMatches(path, sectArg))
      {
	booleanValue = m_booleanValues[i];
	return;
      }
  for(BooleanValueVector::size_type i = 0;i < m_repoBooleanValues.size();i++)
    if (m_repoBooleanValues[i].pathMatches(path, sectArg))
      {
	booleanValue = m_repoBooleanValues[i];
	return;
      }
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

bool parseBooleanValue(const StringVector& path,
		       const std::string& sectArg,
		       const std::string& str,
		       const ConfigFilePosInfo& pos)
{
  if (str == "YES" || str == "Yes" || str == "yes")
    return 1;
  if (str == "TRUE" || str == "True" || str == "true")
    return 1;
  if (str == "1")
    return 1;
  if (str == "NO" || str == "No" || str == "no")
    return 0;
  if (str == "FALSE" || str == "False" || str == "false")
    return 0;
  if (str == "0")
    return 0;
  throw ConfigException(ConfigErrorInvalidBooleanValue, buildConfigParamTitle(path, sectArg), pos);
}

