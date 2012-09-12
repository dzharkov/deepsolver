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

#ifndef DEEPSOLVER_CLI_PARSER_H
#define DEEPSOLVER_CLI_PARSER_H

/**\brief The extensible command line parser*/
class CliParser
{
public:
  struct Key
  {
    Key()
      : used(0) {}

    StringVector names;
    std::string argName, argValue;
    bool used;
    std::string descr;
  }; //struct Key;

  typedef std::list<Key> KeyList;
  typedef std::vector<Key> KeyVector;

public:
  struct Param
  {
    Param()
      : clusterNum(0) {}

    Param(const std::string v)
      : clusterNum(0), value(v) {}

    size_t clusterNum;
    std::string value;
  }; //struct Param;

  typedef std::list<Param> ParamList;
  typedef std::vector<Param> ParamVector;

public:
  CliParser(const std::string& prefix)
    : m_prefix(prefix) {}

  virtual ~CliParser() {}

public:
  void init(int argc, char* argv[]);
  void parse();
  void printHelp(std::ostream& s) const;
  bool wasKeyUsed(const std::string& keyName, std::string& arg);
  void addKey(const std::string& name, const std::string& descr);
  void addKey(const std::string& name, const std::string& argName, const std::string& descr);
  void addKeyDoubleName(const std::string& name1, const std::string& name2, const std::string& descr);
  void addKeyDoubleName(const std::string& name1, const std::string& name2, const std::string& argName, const std::string& descr);

protected:
  virtual size_t recognizeCluster(const StringVector& params, int& mode) const;
  virtual void parseCluster(const StringVector& cluster, int& mode);

protected:
  KeyVector::size_type findKey(const std::string& name) const;
  bool hasKeyArgument(const std::string& name) const;

protected:
  virtual void stopNoPrgName() const;
  virtual void stopMissedArgument(const std::string& keyName) const;

private:
  const std::string m_prefix;
  std::string m_prgName;
  ParamVector m_params;

public:
  KeyVector keys;
  StringVector files;
}; //class CliParser;

#endif //DEEPSOLVER_CLI_PARSER_H;
