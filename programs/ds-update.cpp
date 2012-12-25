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
#include"OperationCore.h"
#include"Messages.h"
#include"IndexFetchProgress.h"

static CliParser cliParser;

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

void parseCmdLine(int argc, char* argv[])
{
  Messages(std::cout).dsUpdateInitCliParser(cliParser);
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
      Messages(std::cout).dsUpdateHelp(cliParser);
      exit(EXIT_SUCCESS);
    }
}

int main(int argc, char* argv[])
{
  messagesProgramName = "ds-update";
  setlocale(LC_ALL, "");
  parseCmdLine(argc, argv);
  initLogging(cliParser.wasKeyUsed("--debug")?LOG_DEBUG:LOG_INFO, cliParser.wasKeyUsed("--log"));
  try{
    AlwaysTrueContinueRequest alwaysTrueContinueRequest;
    if (!cliParser.wasKeyUsed("--log"))
      Messages(std::cout).dsUpdateLogo();
    ConfigCenter conf;
    conf.loadFromFile(DEFAULT_CONFIG_FILE_NAME);
    conf.loadFromDir(DEFAULT_CONFIG_DIR_NAME);
    conf.commit();
    if (!cliParser.wasKeyUsed("--log"))
      Messages(std::cout).introduceRepoSet(conf);
    OperationCore core(conf);
    IndexFetchProgress progress(std::cout, cliParser.wasKeyUsed("--log"));
    core.fetchIndices(progress, alwaysTrueContinueRequest);
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
  catch(const CurlException& e)
    {
      Messages(std::cerr).onCurlError(e);
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
