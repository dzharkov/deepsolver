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

class DsPatchCliParser: public CliParser
{
public:
  /**\brief The default constructor*/
  DsPatchCliParser() {}

  /**\brief The destructor*/
  virtual ~DsPatchCliParser() {}

protected:
  /**\brief Recognizes cluster of command line arguments
   *
   * This is a custom implementation recognizing "--add" and "--del" sequences.
   *
   * \param [in] params The list of all arguments potentially included into cluster
   * \param [in/out] mode The additional mode variable with user-defined purpose
   *
   * \return The number of additional (excluding first) items in provided vector making the cluster
   */
  size_t recognizeCluster(const StringVector& params, int& mode) const
  {
  assert(!params.empty());
  if (mode != 0)//We are already after "--" key;
    return 0;
  if (params[0] != "--add" && params[0] != "--del")
    return CliParser::recognizeCluster(params, mode);
  size_t ending = 1;
  while (ending < params.size() &&
	 params[ending] != "--add" &&
	 params[ending] != "--del" &&
	 params[ending] != "--" &&
	 findKey(params[ending]) == (KeyVector::size_type)-1)
    ending++;
  ending--;
  if (ending < 1)
    throw CliParserException(CliParserException::MissedArgument, params[0]);
  return ending;
  }

  /**\brief Parses one cluster
   *
   * The This implementation of this method parses arguments according to user-defined table and takes into account "--add" and "--del" sequences.
   *
   * \param [in] cluster The arguments of one cluster to parse
   * \param [in/out] mode The additional mode variable with user-defined purpose
   */
  void parseCluster(const StringVector& cluster, int& mode)
  {
    if (mode != 0 || (cluster[0] != "--add" && cluster[0] != "--del"))
      {
	CliParser::parseCluster(cluster, mode);
	return;
      }
    assert(cluster.size() > 1);
    assert(cluster[0] == "--add" || cluster[0] == "--del");
    if (cluster[0] == "--add")
      for(StringVector::size_type i = 1;i < cluster.size();i++)
	filesToAdd.push_back(cluster[i]); else 
      for(StringVector::size_type i = 1;i < cluster.size();i++)
	filesToRemove.push_back(cluster[i]);
  }

public:
  StringVector filesToAdd, filesToRemove;
}; //class DsPatchCliParser;

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
//FIXME:static CmdLineParser cmdLineParser;

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

int install(int argc, char* argv[])
{
  assert(argc > 2);
  logMsg(LOG_DEBUG, "recognized user request to install packages");
  UserTask userTask;
  StringVector params;
  //  cmdLineParser.parseInstallArgs(argc, argv, 2, userTask.itemsToInstall, params);
  //FIXME:URLs must be filtered out;
  assert(!userTask.itemsToInstall.empty());
  logMsg(LOG_DEBUG, "Recognized %zu items to install:", userTask.itemsToInstall.size());
  for(UserTaskItemToInstallVector::size_type i = 0;i < userTask.itemsToInstall.size();i++)
    logMsg(LOG_DEBUG, "%s", userTask.itemsToInstall[i].toString().c_str());
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

int listAvailablePackages(int argc, char* argv[])
{
  logMsg(LOG_DEBUG, "Recognized request to list known packages");
  bool noInstalled = 0, noRepoAvailable = 0, printBuildTime = 0;
  assert(argc >= 2);
  for(int i = 2;i < argc;i++)
    {
      const std::string value(argv[i]);
	if (value == "--no-installed")
	  noInstalled = 1;
	if (value == "--no-repo")
	  noRepoAvailable = 1;
	if (value == "--buildtime")
printBuildTime = 1;
    }
  InfoCore core(conf);
  PkgVector pkgs;
  core.listKnownPackages(pkgs, noInstalled, noRepoAvailable);
  StringVector s;
  s.resize(pkgs.size());
  for(PkgVector::size_type i = 0;i < pkgs.size();i++)
    {
      std::ostringstream ss;
      ss << pkgs[i].name << "-";
      //      if (pkgs[i].epoch > 0)
      //	ss << pkgs[i].epoch << ":";
      ss << pkgs[i].version << "-" << pkgs[i].release;
if (printBuildTime)
ss << " (" << pkgs[i].buildTime << ")";
      s[i] = ss.str();
    }
  std::sort(s.begin(), s.end());
  for(StringVector::size_type i = 0;i < s.size();i++)
    std::cout << s[i] << std::endl;
  return 0;
}

int main(int argc, char* argv[])
{
  //  initLogging("/tmp/ds.log", LOG_DEBUG);//FIXME:
  if (!loadConfiguration())
    return 1;
  if (argc < 2)
    return 0;
  if (std::string(argv[1]) == "update")
    return fetchIndices();
  if (std::string(argv[1]) == "install")
    return install(argc, argv);
  if (std::string(argv[1]) == "ls")
    return listAvailablePackages(argc, argv);
  return 1;
}

