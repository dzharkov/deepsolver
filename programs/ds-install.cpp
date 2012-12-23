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
#include"TransactionProgress.h"
#include"Messages.h"

class DsInstallCliParser: public CliParser
{
public:
  /**\brief The default constructor*/
  DsInstallCliParser() {}

  /**\brief The destructor*/
  virtual ~DsInstallCliParser() {}

protected:
  size_t recognizeCluster(const StringVector& params, int& mode) const
  {
  assert(!params.empty());
  if (mode == 0 && params[0] == "--")
    return 0;
  if (mode == 0 && findKey(params[0]) != (KeyVector::size_type)-1)
    return CliParser::recognizeCluster(params, mode);
  if (params.size() < 3 ||
      (params[1] != '=' && params[1] != '<=' && params[1] != '>=' && params[1] != '<' && params[1] != '>'))
    return CliParser::recognizeCluster(params, mode);
  return 2;
  }

  void parseCluster(const StringVector& cluster, int& mode)
  {
    if (mode == 0 && cluster.size() == 1 && cluster[0] == "--")
      {
	mode = 1;
	return ;
      }
    if (mode == 0 && findKey(params[0]) != (KeyVector::size_type)-1)
      {
	CliParser::parseCluster(cluster, mode);
	return;
      }
    if(cluster.size() != 1 && cluster.size() != 3)
      {
	CliParser::parseCluster(cluster, mode);
	return;
      }
    if (cluster.size() == 1)
      {
	userTask.itemsToInstall.push_back(UserTaskItemToInstall(cluster[0]));
	return;
      }
    VerDir verDir = VerNone;
    if (cluster[1] == "=")
      verDir = VerEquals; else
      if (cluster[1] == "<=")
	verDir = VerLess | VerEquals; else
	if (cluster[1] == ">=")
	  verDir = VerGrater | VerEquals; else
	  if (cluster[1] == "<")
	    verDir = VerLess; else 
	    if (cluster[1] == ">")
	      verDir = VerGreater; else
	      {
		assert(0);
	      }
    userTask.itemsToInstall.push_back(UserTaskItemToInstall(cluster[0], verDir, cluster[2]));
  }

public:
  UserTask userTask;
}; //class DsInstallCliParser;

static DsInstallCliParser cliParser;

void parseCmdLine(int argc, char* argv[])
{
  Messages(std::cout).dsInstallInitCliParser(cliParser);
  try {
    cliParser.init(argc, argv);
    cliParser.parse();
  }
  catch (const CliParserException& e)
    {
      switch (e.getCode())
	{
	case CliParserException::NoPrgName:
	  Messages(std::cerr).onMissedProgramName();
	  exit(EXIT_FAILURE);
	case CliParserException::MissedArgument:
	  Messages(std::cout).onMissedCommandLineArgument(e.getArg());
	  exit(EXIT_FAILURE);
	default:
	  assert(0);
	} //switch();
    }
  if (cliParser.wasKeyUsed("--help"))
    {
      Messages(std::cout).dsInstallHelp(cliParser);
      exit(EXIT_SUCCESS);
    }
}

int main(int argc, char* argv[])
{
  messagesProgramName = "ds-install";
  setlocale(LC_ALL, "");
  parseCmdLine(argc, argv);
  initLogging(cliParser.wasKeyUsed("--debug")?LOG_DEBUG:LOG_INFO, cliParser.wasKeyUsed("--log"));
  try{
    TransactionProgress transactionProgress(std::cout, cliParser.wasKeyUsed("--log"));
    ConfigCenter conf;
    conf.loadFromFile(DEFAULT_CONFIG_FILE_NAME);
    conf.commit();
    OperationCore core(conf);
  logMsg(LOG_DEBUG, "Recognized %zu items to install:", cliParser.userTask.itemsToInstall.size());
  for(UserTaskItemToInstallVector::size_type i = 0;i < cliParser.userTask.itemsToInstall.size();i++)
    logMsg(LOG_DEBUG, "%s", cliParser.userTask.itemsToInstall[i].toString().c_str());
  core.transaction(transactionProgress, cliParser.userTask);
  }
  catch (const ConfigFileException& e)
    {
      Messages(std::cerr).onConfigSyntaxError(e);
      return EXIT_FAILURE;
    }
  catch (const ConfigException& e)
    {
      Messages(std::cerr).onConfigError(e);
      return EXIT_FAILURE;
    }
  catch(const TaskException& e)
    {
      Messages(std::cerr).onTaskError(e);
      return EXIT_FAILURE;
    }
  catch(const OperationException& e)
    {
      Messages(std::cerr).onOperationError(e);
      return EXIT_FAILURE;
    }
  catch(const CurlException& e)
    {
      Messages(std::cerr).onCurlError(e);
      return EXIT_FAILURE;
    }
  catch(const RpmException& e)
    {
      Messages(std::cerr).onRpmError(e);
      return EXIT_FAILURE;
    }
  catch(const SystemException& e)
    {
      Messages(std::cerr).onSystemError(e);
      return EXIT_FAILURE;
    }
  catch(const NotImplementedException& e)
    {
      std::cerr << "Feature not implemented:" << e.getMessage() << std::endl;
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
