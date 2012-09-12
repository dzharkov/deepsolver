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
#include"CliParser.h"

void CliParser::init(int argc, char* argv[])
{
  assert(argv != NULL);
  if (argc < 1)
    stopNoPrgName();
  m_prgName = argv[0];
  for(int i = 1;i < argc;i++)
    m_params.push_back(Param(argv[i]));
}

void CliParser::parse()
{
  StringVector params;
  for(ParamVector::size_type i = 0;i < m_params.size();i++)
    params.push_back(m_params[i].value);
  StringVector::size_type pos = 0;
  size_t clusterValue = 0;
  /*
   * Cluster recognizing method defines meaning of this mode variable by
   * itself. in default behaviour it shows are we already have '--' key.
   */
  int mode = 0;
  while (pos < m_params.size())
    {
      assert(!params.empty());
      size_t num = recognizeCluster(params, mode) + 1;
      if (num >params.size())
	num = params.size();
      for(ParamVector::size_type i = 0;i < num;i++)
	m_params[pos + i].clusterNum = clusterValue;
      pos += num;
      clusterValue++;
      for(StringVector::size_type i = num;i < params.size();i++)
	params[i - num] = params[i];
      params.resize(params.size() - num);
      assert(params.size() + pos == m_params.size());
    }
  assert(params.empty());
  pos = 0;
  clusterValue = 0;
  mode = 0;
  while(pos < m_params.size())
    {
      params.clear();
      while(pos < m_params.size() && m_params[pos].clusterNum == clusterValue)
	params.push_back(m_params[pos++].value);
      parseCluster(params, mode);
      clusterValue++;
    }
}

void CliParser::printHelp(std::ostream& s) const
{
  StringVector lines;
  for(KeyVector::size_type i = 0;i < keys.size();i++)
    {
      const Key& key = keys[i];
      std::string line;
      if (keys[i].argName.empty())
	{
	  for(StringVector::size_type k = 0;k < key.names.size();k++)
	    if (k !=0)
	      line += ", " + key.names[k]; else 
	      line += key.names[k];
	} else 
	{
	  for(StringVector::size_type k = 0;k < key.names.size();k++)
	    if (k !=0)
	      line += ", " + key.names[k] + " " + key.argName; else 
	      line += key.names[k] + " " + key.argName;
	}
      lines.push_back(line);
    }
  size_t maxLen = 0;
  for(StringVector::size_type i = 0;i < lines.size();i++)
    if (lines[i].length() > maxLen)
      maxLen = lines[i].length();
  for(StringVector::size_type i = 0;i < lines.size();i++)
    {
      s << lines[i];
      for(size_t k = lines[i].length();k < maxLen;k++)
	s << " ";
      s << " - " << keys[i].descr << std::endl;
    }
}

bool CliParser::wasKeyUsed(const std::string& keyName, std::string& arg) const
{
  KeyVector::size_type index = findKey(keyName);
  if (index == (KeyVector::size_type)-1)
    return 0;
  assert(index < keys.size());
  if (!keys[index].used)
    return 0;
  if (!keys[index].argName.empty())
    arg = keys[index].argValue;
}

bool CliParser::wasKeyUsed(const std::string& keyName) const
{
  std::string arg;
  return wasKeyUsed(keyName, arg);
}

void CliParser::addKey(const std::string& name, const std::string& descr)
{
  Key key;
  key.names.push_back(name);
  key.descr = descr;
  keys.push_back(key);
}

void CliParser::addKey(const std::string& name, const std::string& argName, const std::string& descr)
{
  Key key;
  key.names.push_back(name);
  key.argName = argName;
  key.descr = descr;
  keys.push_back(key);
}

void CliParser::addKeyDoubleName(const std::string& name1, const std::string& name2, const std::string& descr)
{
  Key key;
  key.names.push_back(name1);
  key.names.push_back(name2);
  key.descr = descr;
  keys.push_back(key);
}

void CliParser::addKeyDoubleName(const std::string& name1, const std::string& name2, const std::string& argName, const std::string& descr)
{
  Key key;
  key.names.push_back(name1);
  key.names.push_back(name2);
  key.argName = argName;
  key.descr = descr;
  keys.push_back(key);
}

size_t CliParser::recognizeCluster(const StringVector& params, int& mode) const
{
  assert(!params.empty());
  if (mode != 0)
    return 0;
  if (params[0] == "--")
    {
      mode = 1;
      return 0;
    }
  if (!hasKeyArgument(params[0]))
    return 0;
  if (params.size() < 2)
    stopMissedArgument(params[0]);
  return 1;
}

void CliParser::parseCluster(const StringVector& cluster, int& mode)
{
  assert(!cluster.empty());
  if (cluster[0] == "--")
    {
      mode = 1;
      return;
    }
  const KeyVector::size_type pos = findKey(cluster[0]);
  if (mode == 1 || pos == (KeyVector::size_type)-1)
    {
      for(StringVector::size_type i = 0;i < cluster.size();i++)
	files.push_back(cluster[i]);
      return;
    }
  assert(pos < keys.size());
  keys[pos].used = 1;
  if (!keys[pos].argName.empty())
    {
      assert(cluster.size() > 1);
      keys[pos].argValue = cluster[1];
    }
}

CliParser::KeyVector::size_type CliParser::findKey(const std::string& name) const
{
  assert(!name.empty());
  for(KeyVector::size_type i = 0;i < keys.size();i++)
    for(StringVector::size_type k = 0;k < keys[i].names.size();k++)
      if (keys[i].names[k] == name)
	return i;
  return (KeyVector::size_type)-1;
}

bool CliParser::hasKeyArgument(const std::string& name) const
{
  assert(!name.empty());
  KeyVector::size_type pos = findKey(name);
  if (pos == (KeyVector::size_type)-1)
    return 0;
  assert(pos < keys.size());
  return !keys[pos].argName.empty();
}

void CliParser::stopNoPrgName() const
{
  std::cerr << m_prefix << "Command line arguments list too short, it must contain at least one item (program name)" << std::endl;
  exit(EXIT_FAILURE);
}

void CliParser::stopMissedArgument(const std::string& keyName) const
{
  std::cerr << m_prefix << "Command line key \'" << keyName << "\' requires an argument, but it is the last parameter" << std::endl;
  exit(EXIT_FAILURE);
}
