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
#include"OperationCore.h"
#include"InfoCore.h"
#include"Messages.h"
#include"IndexFetchProgress.h"

class AlwaysTrueContinueRequest: public AbstractOperationContinueRequest
{
public:
  AlwaysTrueContinueRequest() {}
  virtual ~AlwaysTrueContinueRequest() {}

public:
  bool onContinueOperationRequest() const
  {
    return 1;
  }
}; //class AlwaysTrueContinueRequest; 

static AlwaysTrueContinueRequest alwaysTrueContinueRequest;

static ConfigCenter conf;

bool loadConfiguration()
{
  try{
  conf.loadFromFile("/tmp/ds.ini");
  conf.commit();
  }
  catch (const ConfigFileException& e)
    {
      Messages(std::cerr).onConfigSyntaxError(e);
      return 0;
    }
  catch (const ConfigException& e)
    {
      Messages(std::cerr).onConfigError(e);
      return 0;
    }
  catch(const SystemException& e)
    {
      Messages(std::cerr).onSystemError(e);
      return 0;
    }
  return 1;
}


int fetchIndices()
{
  logMsg(LOG_DEBUG, "recognized user request to update package indices");
  OperationCore core(conf);
  try {
    IndexFetchProgress progress(std::cout);
    core.fetchIndices(progress, alwaysTrueContinueRequest);
  }
  catch (const OperationException& e)
    {
      Messages(std::cerr).onOperationError(e);
      return 1;
    }
  catch(const SystemException& e)
    {
      Messages(std::cerr).onSystemError(e);
      return 1;
    }
  catch(const CurlException& e)
    {
      Messages(std::cerr).onCurlError(e);
      return 1;
    }
  return 0;
}

int install(const UserTask& userTask)
{
  logMsg(LOG_DEBUG, "recognized user request to install packages");
  OperationCore core(conf);
  try {
    core.doInstallRemove(userTask);
  }
  catch (const OperationException& e)
    {
      Messages(std::cerr).onOperationError(e);
      return 1;
    }
  catch(const SystemException& e)
    {
      Messages(std::cerr).onSystemError(e);
      return 1;
    }
  catch(const CurlException& e)
    {
      Messages(std::cerr).onCurlError(e);
      return 1;
    }
  return 0;
}

int listAvailablePackages()
{
  logMsg(LOG_DEBUG, "Recognized request to list all available packages");
  InfoCore core(conf);
  PkgVector pkgs;
  core.availablePackages(pkgs);
  StringVector s;
  s.resize(pkgs.size());
  for(PkgVector::size_type i = 0;i < pkgs.size();i++)
    {
      std::ostringstream ss;
      ss << pkgs[i].name << "-";
      //      if (pkgs[i].epoch > 0)
      //	ss << pkgs[i].epoch << ":";
      ss << pkgs[i].version << "-" << pkgs[i].release;
      s[i] = ss.str();
    }
  std::sort(s.begin(), s.end());
  for(StringVector::size_type i = 0;i < s.size();i++)
    std::cout << s[i] << std::endl;
  return 0;
}

int main(int argc, char* argv[])
{
  initLogging("/tmp/ds.log", LOG_DEBUG);//FIXME:
  if (!loadConfiguration())
    return 1;
  if (argc < 2)
    return 0;
  if (std::string(argv[1]) == "update")
    return fetchIndices();
  if (std::string(argv[1]) == "install")
    return install(UserTask());
  if (std::string(argv[1]) == "list")
    return listAvailablePackages();
  return 1;
}

