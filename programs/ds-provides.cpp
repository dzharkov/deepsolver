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
#include"IndexCore.h"
#include"CliParser.h"

#define PREFIX "ds-provides:"

static RepoParams params;
static CliParser cliParser;

class IndexReconstructionListener: public AbstractIndexConstructionListener
{
public:
  IndexReconstructionListener(bool suppress)
    : m_suppress(suppress) {}

  virtual ~IndexReconstructionListener() {}

public:
  void onReferenceCollecting(const std::string& path)
  {
    if (m_suppress)
      return;
    std::cout << "Reading references in " << path << std::endl;
  }

  void onPackageCollecting(const std::string& path) {}

  void onProvidesCleaning() 
  {
    if (m_suppress)
      return;
    std::cout << "Performing provides filtering" << std::endl;
  }

  void onChecksumWriting()
  {
    if (m_suppress)
      return;
    std::cout << "Writing checksum file" << std::endl;
  }

  void onChecksumVerifying() 
  {
    if (m_suppress)
      return;
    std::cout << "Verifying checksums" << std::endl;
  }

  void onPatchingFile(const std::string& fileName)
  {
    if (m_suppress)
      return;
    std::cout << "Fixing references in  " << fileName << std::endl;
  }

  void onNoTwiceAdding(const std::string& fileName) {}

private:
  bool m_suppress;
}; //class IndexReconstructionListener;

void initCliParser()
{
  cliParser.addKeyDoubleName("-s", "--ref-sources", "LIST", "take additional requires/conflicts for provides filtering in listed directories (list should be colon-delimited)");
  cliParser.addKeyDoubleName("-h", "--help", "print this help screen and exit");
  cliParser.addKey("--log", "print log to console instead of user progress information");
  cliParser.addKey("--debug", "relax filtering level for log output");
}

void printLogo()
{
  std::cout << "ds-provides: The utility to fix provides list in Deepsolver repository" << std::endl;
    std::cout << "Version: " << PACKAGE_VERSION << std::endl;
  std::cout << std::endl;
}

void printHelp()
{
  printLogo();
  printf("%s", 
	 "Usage:\n"
	 "\tds-provides [OPTIONS] INDEX_DIR\n"
	 "\n"
	 "Where:\n"
	 "INDEX_DIR          - directory with indices to fix references in\n"
	 "\n"
	 "Valid options are:\n");
  cliParser.printHelp(std::cout);
}

void splitColonDelimitedList(const std::string& str, StringVector& res)
{
  std::string s;
  for(std::string::size_type i = 0;i < str.length();i++)
    {
      if (str[i] == ':')
	{
	  if (s.empty())
	    continue;
	  res.push_back(s);
	  s.erase();
	  continue;
	}
      s += str[i];
    } //for();
  if (!s.empty())
    res.push_back(s);
}

void parseCmdLine(int argc, char* argv[])
{
  try {
    cliParser.init(argc, argv);
    cliParser.parse();
  }
  catch (const CliParserException& e)
    {
      switch (e.getCode())
	{
	case CliParserException::NoPrgName:
	  std::cerr << PREFIX << "Command line has no program name" << std::endl;
	  exit(EXIT_FAILURE);
	case CliParserException::MissedArgument:
	  std::cerr << PREFIX << "Command line argument \'" << e.getArg() << "\' requires additional parameter" << std::endl;exit(EXIT_FAILURE);
	  exit(EXIT_FAILURE);
	default:
	  assert(0);
	} //switch();
    }
  if (cliParser.wasKeyUsed("--help"))
    {
      printHelp();
	exit(EXIT_SUCCESS);
    }
  if (cliParser.files.empty())
    cliParser.files.push_back(".");
  if (cliParser.files.size() > 1)
    {
      std::cerr << PREFIX << "Extra command line argument \'" << cliParser.files[1] << "\'" << std::endl;
      exit(EXIT_FAILURE);
    }
  params.indexPath = cliParser.files[0];
  std::string arg;
  if (cliParser.wasKeyUsed("--ref-sources", arg))
    splitColonDelimitedList(arg, params.providesRefsSources);
}

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "");
  initCliParser();
  parseCmdLine(argc, argv);
  initLogging(cliParser.wasKeyUsed("--debug")?LOG_DEBUG:LOG_INFO, cliParser.wasKeyUsed("--log"));
  try {
    if (!cliParser.wasKeyUsed("--log"))
      printLogo();
    params.readInfoFile(Directory::mixNameComponents(params.indexPath, REPO_INDEX_INFO_FILE));
    IndexReconstructionListener listener(cliParser.wasKeyUsed("--log"));
    IndexCore indexCore(listener);
    indexCore.refilterProvides(params);
  }
  catch(const DeepsolverException& e)
    {
      logMsg(LOG_CRIT, "%s error:%s", e.getType().c_str(), e.getMessage().c_str());
      if (!cliParser.wasKeyUsed("--log"))
	std::cerr << "ERROR:" << e.getMessage() << std::endl;
      return EXIT_FAILURE;
    }
  catch(std::bad_alloc)
    {
      logMsg(LOG_CRIT, "No enough memory");
      if (!cliParser.wasKeyUsed("--log"))
	std::cerr << "ERROR:No enough memory" << std::endl;
	  return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
