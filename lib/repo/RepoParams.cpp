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
#include"RepoParams.h"
#include"InfoFileReader.h"

static std::string booleanValue(bool value)
{
  return value?"yes":"no";
}

bool parseBooleanValue(const std::string& str, const std::string& paramName)
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
  throw InfoFileValueException(InfoFileValueException::InvalidBooleanValue, paramName);
}

static std::string doubleQuotes(const std::string& str)
{
  std::string res;
  for(std::string::size_type i = 0;i < str.length();i++)
    if (str[i] == '\"')
      res += "\"\""; else
      res += str[i];
  return res;
}

static std::string saveStringVector(const StringVector& values)
{
  if (values.empty())
    return "";
  std::string str = "\"" + doubleQuotes(values[0]) + "\"";
  for(StringVector::size_type i = 1;i < values.size();i++)
    {
      str += ":";
      str += "\"" + doubleQuotes(values[i]) + "\"";
    }
  return str;
}

static void parseStringVector(const std::string& str, StringVector& res)
{
  res.clear();
  std::string item;
  bool inQuotes = 0;
  for(std::string::size_type i = 0;i < str.length();i++)
    {
      if (!inQuotes && str[i] == ':')
	{
	  res.push_back(item);
	  item.erase();
	  continue;
	}
      if (!inQuotes && str[i] == '\"')
	{
	  inQuotes = 1;
	  continue;
	}
      if (inQuotes && str[i] == '\"')
	{
	  if (i + 1 < str.length() && str[i + 1] == '\"')
	    {
	      item += "\"";
	      i++;
	      continue;
	    }
	  inQuotes = 0;
	  continue;
	}
      item += str[i];
    }
  if (!res.empty() || !item.empty())
    res.push_back(item);
}

static std::string escapeString(const std::string& s)
{
  std::string res;
  for(std::string::size_type i = 0;i < s.length();i++)
    {
      switch(s[i])
	{
	case '\\':
	  res += "\\\\";
	  break;
	case '#':
	  res += "\\#";
	  break;
	default:
	  res += s[i];
	}; //switch();
    }
  return res;
}

static void writeInfoFileParamsToDisk(const std::string& fileName, const StringToStringMap& params)
{
  std::ostringstream ss;
  time_t t;
  time(&t);
  ss << "# Deepsolver repository index information file" << std::endl;
  ss << "# Generated on " << ctime(&t);
  ss << "#" << std::endl;
  ss << "# This file was created automatically. Do not edit!" << std::endl;
  ss << "#" << std::endl;
  ss << std::endl;
  for(StringToStringMap::const_iterator it = params.begin();it != params.end();it++)
      ss << it->first << " = " << escapeString(it->second) << std::endl;
  File f;
  f.create(fileName);
  f.write(ss.str().c_str(), ss.str().length());
}

void RepoParams::writeInfoFile(const std::string& fileName) const
{
  logMsg(LOG_DEBUG, "Saving info file in \'%s\'", fileName.c_str());
  StringToStringMap params;
  switch(formatType)
    {
    case FormatTypeText:
      params.insert(StringToStringMap::value_type(INFO_FILE_FORMAT_TYPE, INFO_FILE_FORMAT_TYPE_TEXT));
      break;
    case FormatTypeBinary:
      params.insert(StringToStringMap::value_type(INFO_FILE_FORMAT_TYPE, INFO_FILE_FORMAT_TYPE_BINARY));
      break;
    default:
      assert(0);
    }; //switch(formatType);
  switch(compressionType)
    {
    case CompressionTypeNone:
      params.insert(StringToStringMap::value_type(INFO_FILE_COMPRESSION_TYPE, INFO_FILE_COMPRESSION_TYPE_NONE));
      break;
    case CompressionTypeGzip:
      params.insert(StringToStringMap::value_type(INFO_FILE_COMPRESSION_TYPE, INFO_FILE_COMPRESSION_TYPE_GZIP));
      break;
    default:
      assert(0);
    }; //switch(compressionType);
  params.insert(StringToStringMap::value_type(INFO_FILE_VERSION, version));
  params.insert(StringToStringMap::value_type(INFO_FILE_FILTER_PROVIDES_BY_DIRS, saveStringVector(filterProvidesByDirs)));
  params.insert(StringToStringMap::value_type(INFO_FILE_FILTER_PROVIDES_BY_DIRS, saveStringVector(filterProvidesByDirs)));
  params.insert(StringToStringMap::value_type(INFO_FILE_FILTER_PROVIDES_BY_REFS, booleanValue(filterProvidesByRefs)));
  params.insert(StringToStringMap::value_type(INFO_FILE_EXCLUDE_REQUIRES, saveStringVector(excludeRequiresRegExp)));
  params.insert(StringToStringMap::value_type(INFO_FILE_CHANGELOG_BINARY, booleanValue(changeLogBinary)));
  params.insert(StringToStringMap::value_type(INFO_FILE_CHANGELOG_SOURCES, booleanValue(changeLogSources)));
  params.insert(StringToStringMap::value_type(INFO_FILE_MD5SUM, REPO_INDEX_MD5SUM_FILE));
  for(StringToStringMap::const_iterator it = userParams.begin();it != userParams.end();it++)
    params.insert(*it);
  writeInfoFileParamsToDisk(fileName, params);
}

void RepoParams::readInfoFile(const std::string& fileName)
{
  File f;
  logMsg(LOG_DEBUG, "Opening \'%s\' info file in read-only mode", fileName.c_str());
  f.openReadOnly(fileName);
  std::string content;
  f.readTextFile(content);
  InfoFileReader reader;
  StringToStringMap params;
  reader.read(content, params);
  for(StringToStringMap::const_iterator it = params.begin();it != params.end();it++)
    {
      const std::string& name = it->first;
      const std::string& value = it->second;
      logMsg(LOG_DEBUG, "%s:parsing \'%s\' = \'%s\'", fileName.c_str(), name.c_str(), value.c_str());
      //Format type;
      if (trim(name) == INFO_FILE_FORMAT_TYPE)
	{
	  if (trim(value) == INFO_FILE_FORMAT_TYPE_TEXT)
	    formatType = FormatTypeText; else
	    if (trim(value) == INFO_FILE_FORMAT_TYPE_BINARY)
	      formatType = FormatTypeBinary; else
	      throw InfoFileValueException(InfoFileValueException::InvalidFormatType, trim(value));
	  continue;
	} //Format type;
      //Compression type;
      if (trim(name) == INFO_FILE_COMPRESSION_TYPE)
	{
	  if (trim(value) == INFO_FILE_COMPRESSION_TYPE_NONE)
	    compressionType = CompressionTypeNone; else
	    if (trim(value) == INFO_FILE_COMPRESSION_TYPE_GZIP)
	      compressionType = CompressionTypeGzip; else
	      throw InfoFileValueException(InfoFileValueException::InvalidCompressionType, trim(value));
	  continue;
	} //Compression type;
      //Version;
      if (trim(name) == INFO_FILE_VERSION)
	{
	  version = trim(value);
	  continue;
	} //Version;
      //Md5sum;
      if (trim(name) == INFO_FILE_MD5SUM)
	{
	md5sumFileName = trim(value);
	continue;
	} //Md5sum;
      // Change log binary;
      if (trim(name) == INFO_FILE_CHANGELOG_BINARY)
	{
	  changeLogBinary = parseBooleanValue(trim(value), trim(name));
	  continue;
	} //Change log binary;
      //Change log sources;
      if (trim(name) == INFO_FILE_CHANGELOG_SOURCES)
	{
	  changeLogSources = parseBooleanValue(trim(value), trim(name));
	  continue;
	} //Change log sources;
      //Filter provides by references;
      if (trim(name) == INFO_FILE_FILTER_PROVIDES_BY_REFS)
	{
	  filterProvidesByRefs = parseBooleanValue(trim(value), trim(name));
	  continue;
	}
      //Filter provides by dirs;
      if (trim(name) == INFO_FILE_FILTER_PROVIDES_BY_DIRS)
	{
	  parseStringVector(trim(value), filterProvidesByDirs);
	  continue;
	} //Filter provides by dirs;
      //Exclude requires;
      if (trim(name) == INFO_FILE_EXCLUDE_REQUIRES)
	{
	  parseStringVector(trim(value), excludeRequiresRegExp);
	  continue;
	}
      userParams.insert(StringToStringMap::value_type(trim(name), trim(value)));
    } //for(values);
}
