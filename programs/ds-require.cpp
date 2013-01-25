/*
   Copyright 2011-2013 ALT Linux
   Copyright 2011-2013 Michael Pozhidaev

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

static CliParser cliParser;
static NamedPkgRel rel;

void parseCmdLine(int argc, char* argv[])
{
  Messages(std::cout).dsRequireInitCliParser(cliParser);
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
      Messages(std::cout).dsRequireHelp(cliParser);
      exit(EXIT_SUCCESS);
    }
  if (cliParser.files.size() != 1 && cliParser.files.size() != 3)
    {
      Messages(std::cerr).dsRequireOnInvalidInput();
      exit(EXIT_FAILURE);
    }
  rel.pkgName = cliParser.files[0];
  if (cliParser.files.size() == 3)
    {
      const std::string dir = cliParser.files[1];
      if (dir == "<")
	rel.type = VerLess; else
	if (dir == "<=")
	  rel.type = VerLess | VerEquals; else
	  if (dir == "=")
	    rel.type = VerEquals; else
	    if (dir == ">=")
	      rel.type = VerEquals | VerGreater; else
	      if (dir == ">")
		rel.type = VerGreater; else
		{
		  Messages(std::cerr).dsRequireOnInvalidInput();
		  exit(EXIT_FAILURE);
		}
      rel.ver = cliParser.files[2];
    }
  if (rel.pkgName.empty() || rel.ver.empty())
    {
      Messages(std::cerr).dsRequireOnInvalidInput();
      exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[])
{
  messagesProgramName = "ds-require";
  setlocale(LC_ALL, "");
  parseCmdLine(argc, argv);
  initLogging(cliParser.wasKeyUsed("--debug")?LOG_DEBUG:LOG_INFO, cliParser.wasKeyUsed("--log"));
  try{
    ConfigCenter conf;
    conf.loadFromFile(DEFAULT_CONFIG_FILE_NAME);
    conf.loadFromDir(DEFAULT_CONFIG_DIR_NAME);
    conf.commit();
    OperationCore core(conf);

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
