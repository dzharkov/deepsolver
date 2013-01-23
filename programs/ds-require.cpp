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

static NamedPkgRel rel;

void parseCmdLine(int argc, char* argv[])
{
  CliParser cliParser;
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
  if (cliParser.files.size() != 1 && cliParser.files.size() != 3)
    {
      //FIXME:
      exit(EXIT_FAILURE);
    }
  for()
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
    conf.loadFromDir(DEFAULT_CONFIG_DIR_NAME);
    conf.commit();
    if (!cliParser.wasKeyUsed("--log"))
      Messages(std::cout).dsInstallLogo();
    OperationCore core(conf);
    if (cliParser.userTask.itemsToInstall.empty())
      {
	Messages(std::cerr).onNoPackagesMentionedError();
	return EXIT_FAILURE;
      }
    if (!cliParser.wasKeyUsed("--sat"))
      {
	core.transaction(transactionProgress, cliParser.userTask);
      } else
      {
	const std::string res = core.generateSat(transactionProgress, cliParser.userTask);
	std::cout << std::endl;
	std::cout << res;
      }
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
