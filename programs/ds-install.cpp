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
#include"Messages.h"
#include"transact/TaskException.h"

class DsInstallCliParser: public CliParser
{
public:
  /**\brief The default constructor*/
  DsInstallCliParser() {}

  /**\brief The destructor*/
  virtual ~DsInstallCliParser() {}

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
    //    if (cluster[0] == "--add")
      //      for(StringVector::size_type i = 1;i < cluster.size();i++)
	//	filesToAdd.push_back(cluster[i]); else 
	//      for(StringVector::size_type i = 1;i < cluster.size();i++)
	//	filesToRemove.push_back(cluster[i]);
  }

public:
  UserTask userTask;
}; //class DsPInstallCliParser;

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
    ConfigCenter conf;
    conf.loadFromFile("/tmp/ds.ini");
    conf.commit();
    OperationCore core(conf);
  logMsg(LOG_DEBUG, "Recognized %zu items to install:", cliParser.userTask.itemsToInstall.size());
  for(UserTaskItemToInstallVector::size_type i = 0;i < cliParser.userTask.itemsToInstall.size();i++)
    logMsg(LOG_DEBUG, "%s", cliParser.userTask.itemsToInstall[i].toString().c_str());
    core.doInstallRemove(cliParser.userTask);
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
  catch(const OperationException& e)
    {
      Messages(std::cerr).onOperationError(e);
      return EXIT_FAILURE;
    }
  catch(const SystemException& e)
    {
      Messages(std::cerr).onSystemError(e);
      return EXIT_FAILURE;
    }
  catch(const TaskException& e)
    {
       Messages(std::cerr).onTaskError(e);
       return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
