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

class CliParser
{
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

protected:
  size_t recognizeCluster(const StringVector& params) const;

private:
  void stopNoPrgName() const;

private:
  const std::string m_prefix;
  std::string m_prgName;
  ParamVector m_params;
}; //class CliParser;

#endif //DEEPSOLVER_CLI_PARSER_H;
