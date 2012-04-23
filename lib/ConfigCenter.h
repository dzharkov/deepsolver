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

#ifndef DEEPSOLVER_CONFIG_CENTER_H
#define DEEPSOLVER_CONFIG_CENTER_H

#include"DeepsolverException.h"
#include"ConfigFile.h"
#include"ConfigData.h"

enum {
  ConfigErrorUnknownParam = 0
};

class ConfigException: public DeepsolverException
{
public:
  ConfigException(int code, const std::string& arg, const ConfigFilePosInfo& pos)
    : m_code(code),
      m_arg(arg),
      m_fileName(pos.fileName),
      m_lineNumber(pos.lineNumber),
      m_line(pos.line) {}

public:
  int getCode() const
  {
    return m_code;
  }

  std::string getArg() const
  {
    return m_arg;
  }

  std::string getFileName() const
  {
    return m_fileName;
  }

  size_t getLineNumber() const
  {
    return m_lineNumber;
  }

  std::string getLine() const
  {
    return m_line;
  }

  std::string getType() const
  {
    return "config";
  }

  std::string getMessage() const
  {
    return "FIXME";
  }



private:
  const int m_code;
  const std::string m_arg;
  const std::string m_fileName;
  const size_t m_lineNumber;
  const std::string m_line;
}; //class DeepsolverException;

class ConfigCenter: private AbstractConfigFileHandler
{
public:
  ConfigCenter() {}
  virtual ~ConfigCenter() {}

public:
  void loadFromFile(const std::string& fileName);

  const ConfRoot& root() const
  {
    return m_root;
  }

private:
    void onRepoConfigValue(const StringVector&path,
			   const std::string& sectArg,
			   const std::string& value,
			   bool adding, 
			   const ConfigFilePosInfo& pos);

private://AbstractConfigFileHandler;
  void onConfigFileValue(const StringVector& path, 
			 const std::string& sectArg,
			 const std::string& value,
			 bool adding,
			 const ConfigFilePosInfo& pos);

private:
  ConfRoot m_root;
}; //class ConfigCenter;

#endif //DEEPSOLVER_CONFIG_CENTER_H;
