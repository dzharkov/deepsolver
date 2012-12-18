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

#define DELIMITERS ";"

static std::string buildConfigParamTitle(const StringVector& path, const std::string& sectArg);
static bool parseBooleanValue(const StringVector& path,
		       const std::string& sectArg,
		       const std::string& str,
		       const ConfigFilePosInfo& pos);
static unsigned int parseUintValue(const StringVector& path,
		       const std::string& sectArg,
		       const std::string& str,
				   const ConfigFilePosInfo& pos);

void ConfigCenter::initValues()
{
  addNonEmptyStringParam3("core", "dir", "pkg-data", m_root.dir.pkgData);
  addNonEmptyStringListParam3("core", "dir", "pkg-data", m_root.os.transactReadAhead);
}

void ConfigCenter::initRepoValues()
{
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
      m_stringValues.push_back(stringValue);
      //Vendor;
      stringValue.path[1] = "vendor";
      stringValue.value = &repo.vendor;
      stringValue.canBeEmpty = 1;
      m_stringValues.push_back(stringValue);
      //Arch;
      StringListValue stringListValue;
      stringListValue.sectArg = repo.name;
      stringListValue.path.push_back("repo");
      stringListValue.path.push_back("arch");
      stringListValue.delimiters = DELIMITERS;
      stringListValue.canContainEmptyItem = 0;
      stringListValue.value = &repo.arch;
      m_stringListValues.push_back(stringListValue);
      //Components;
      stringListValue.path[1] = "components";
      stringListValue.value = &repo.components;
      m_stringListValues.push_back(stringListValue);
      //Enabled;
      BooleanValue booleanValue;
      booleanValue.path.push_back("repo");
      booleanValue.path.push_back("enabled");
      booleanValue.value = &repo.enabled;
      m_booleanValues.push_back(booleanValue);
      //Take*;
      booleanValue.path[1] = "take-descr";
      booleanValue.value = &repo.takeDescr;
      m_booleanValues.push_back(booleanValue);
      booleanValue.path[1] = "take-file-list";
      booleanValue.value = &repo.takeFileList;
      m_booleanValues.push_back(booleanValue);
      booleanValue.path[1] = "take-sources";
      booleanValue.value = &repo.takeSources;
      m_booleanValues.push_back(booleanValue);
    }
}

void ConfigCenter::commit()
{
  logMsg(LOG_DEBUG, "config:about to commit configuration values");
  m_root.dir.pkgData = trim(m_root.dir.pkgData);
  for(StringVector::size_type i = 0;i < m_root.os.transactReadAhead.size();i++)
    m_root.os.transactReadAhead[i] = trim(m_root.os.transactReadAhead[i]);
  for(ConfRepoVector::size_type i = 0;i < m_root.repo.size();i++)
    {
      ConfRepo& repo = m_root.repo[i];
      repo.name = trim(repo.name);
      repo.url = trim(repo.url);
      repo.vendor = trim(repo.vendor);
      for(StringVector::size_type k = 0;k < repo.arch.size();k++)
	repo.arch[k] = trim(repo.arch[k]);
      for(StringVector::size_type k = 0;k < repo.components.size();k++)
	repo.components[k] = trim(repo.components[k]);
    }
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    if (!m_stringValues[i].canBeEmpty && trim(*m_stringValues[i].value).empty())
      throw ConfigException(ConfigException::ValueCannotBeEmpty, m_stringValues[i].pathToString());
  logMsg(LOG_DEBUG, "config:commit completed");
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
      if (trim(sectArg).empty())
	throw NotImplementedException("Empty configuration file section argument");
      ConfRepoVector::size_type i = 0;
      while(i < m_root.repo.size() && m_root.repo[i].name != sectArg)
	i++;
      if (i >= m_root.repo.size())
	m_root.repo.push_back(ConfRepo(sectArg));
      m_stringValues.clear();
      m_stringListValues.clear();
      m_booleanValues.clear();
      m_uintValues.clear();
      initValues();
      initRepoValues();
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
  if (paramType == ValueTypeUint)
    {
      processUintValue(path, sectArg, value, adding, pos);
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
	throw ConfigException(ConfigException::ValueCannotBeEmpty, buildConfigParamTitle(path, sectArg), pos.fileName, pos.lineNumber, pos.line);
      v.push_back(item);
      item.erase();
    }
  if (!stringListValue.canContainEmptyItem && trim(item).empty())
    throw ConfigException(ConfigException::ValueCannotBeEmpty, buildConfigParamTitle(path, sectArg), pos.fileName, pos.lineNumber, pos.line);
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
    throw ConfigException(ConfigException::AddingNotPermitted, buildConfigParamTitle(path, sectArg), pos.fileName, pos.lineNumber, pos.line);
  v = parseBooleanValue(path, sectArg, trim(value), pos);
}

void ConfigCenter::processUintValue(const StringVector& path, 
				      const std::string& sectArg,
				      const std::string& value,
				      bool adding,
				      const ConfigFilePosInfo& pos)
{
  UintValue uintValue;
  findUintValue(path, sectArg, uintValue);
  assert(uintValue.value != NULL);
  unsigned int& v = *(uintValue.value);
  v = parseUintValue(path, sectArg, value, pos);
}

int ConfigCenter::getParamType(const StringVector& path, const std::string& sectArg, const ConfigFilePosInfo& pos) const
{
  //String;
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    if (m_stringValues[i].pathMatches(path, sectArg))
      return ValueTypeString;
  //StringList;
  for(StringListValueVector::size_type i = 0;i < m_stringListValues.size();i++)
    if (m_stringListValues[i].pathMatches(path, sectArg))
      return ValueTypeStringList;
  //Boolean;
  for(BooleanValueVector::size_type i = 0;i < m_booleanValues.size();i++)
    if (m_booleanValues[i].pathMatches(path, sectArg))
      return ValueTypeBoolean;
  //Unsigned integer;
  for(UintValueVector::size_type i = 0;i < m_uintValues.size();i++)
    if (m_uintValues[i].pathMatches(path, sectArg))
      return ValueTypeUint;
  throw ConfigException(ConfigException::UnknownParam, buildConfigParamTitle(path, sectArg), pos.fileName, pos.lineNumber, pos.line);
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
  assert(0);
}

void ConfigCenter::findUintValue(const StringVector& path, 
				   const std::string& sectArg,
				   UintValue& uintValue)
{
  for(BooleanValueVector::size_type i = 0;i < m_uintValues.size();i++)
    if (m_uintValues[i].pathMatches(path, sectArg))
      {
	uintValue = m_uintValues[i];
	return;
      }
  assert(0);
}

void ConfigCenter::loadFromFile(const std::string& fileName)
{
  logMsg(LOG_DEBUG, "config:reading configuration from \'%s\'", fileName.c_str());
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

void ConfigCenter::addStringListParam3(const std::string& path1,
				   const std::string& path2,
				   const std::string& path3,
				   StringVector& value)
{
  assert(!path1.empty() && !path2.empty() && !path3.empty());
  StringListValue stringListValue(value);
  stringListValue.canContainEmptyItem = 1;
  stringListValue.path.push_back(path1);
  stringListValue.path.push_back(path2);
  stringListValue.path.push_back(path3);
  m_stringListValues.push_back(stringListValue);
}

void ConfigCenter::addNonEmptyStringListParam3(const std::string& path1,
					   const std::string& path2,
					   const std::string& path3,
					   StringVector& value)
{
  assert(!path1.empty() && !path2.empty() && !path3.empty());
  StringListValue stringListValue(value);
  stringListValue.canContainEmptyItem = 0;
  stringListValue.path.push_back(path1);
  stringListValue.path.push_back(path2);
  stringListValue.path.push_back(path3);
  m_stringListValues.push_back(stringListValue);
}

// Static functions;

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
  throw ConfigException(ConfigException::InvalidBooleanValue, buildConfigParamTitle(path, sectArg), pos.fileName, pos.lineNumber, pos.line);
}

unsigned int parseUintValue(const StringVector& path,
		       const std::string& sectArg,
		       const std::string& str,
		       const ConfigFilePosInfo& pos)
{
  if (trim(str).empty())
    throw ConfigException(ConfigException::InvalidUintValue, buildConfigParamTitle(path, sectArg), pos.fileName, pos.lineNumber, pos.line);
  std::istringstream ss(trim(str));
  unsigned int k;
  if (!(ss >> k))
    throw ConfigException(ConfigException::InvalidUintValue, buildConfigParamTitle(path, sectArg), pos.fileName, pos.lineNumber, pos.line);
  return k;
}
