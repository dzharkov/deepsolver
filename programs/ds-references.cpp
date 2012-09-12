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
#include"IndexCore.h"
#include"CliParser.h"

#define PREFIX "ds-references:"

static RepoParams params;
static CliParser cliParser(PREFIX);

class IndexReconstructionListener: public AbstractIndexConstructionListener
{
public:
  IndexReconstructionListener() {}
  virtual ~IndexReconstructionListener() {}

public:
  void onReferenceCollecting(const std::string& path)
  {
    std::cout << "Reading references in " << path << std::endl;
  }

  void onPackageCollecting(const std::string& path) {}
  void onProvidesCleaning() {}

  void onPatchingFile(const std::string& fileName)
  {
    std::cout << "Fixing references in  " << fileName << std::endl;
  }

  void onNoTwiceAdding(const std::string& fileName) {}
}; //class IndexReconstructionListener;

void initCliParser()
{
  cliParser.addKeyDoubleName("-h", "--help", "print this help screen and exit");
  cliParser.addKey("--log", "print log to console instead of user progress information");
  cliParser.addKey("--debug", "relax filtering level for log output");
}

void printLogo()
{
  std::cout << "ds-references: utility to fix provides references in Deepsolver repository" << std::endl;
    std::cout << "Version: " << PACKAGE_VERSION << std::endl;
  std::cout << std::endl;
}

void printHelp()
{
  printLogo();
  printf("%s", 
	 "Usage:\n"
	 "\tds-references [OPTIONS] INDEX_DIR [REFERENCES_SOURCES]\n"
	 "\n"
	 "Where:\n"
	 "\tINDEX_DIR          - directory with indices to fix references in\n"
	 "\tREFERENCES_SOURCES - list of colon-delimited directories to take requires/conflicts from, directory can contain either the repo index or packages files\n"
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
  cliParser.init(argc, argv);
  cliParser.parse();
  if (cliParser.files.empty())
    {
      std::cerr << PREFIX << "index directory was not mentioned" << std::endl;
      exit(EXIT_FAILURE);
    }
  if (cliParser.files.size() > 2)
    {
      std::cerr << PREFIX << "unparsable command line argument \'" << cliParser.files[2] << "\'" << std::endl;
      exit(EXIT_FAILURE);
    }
  params.indexPath = cliParser.files[0];
  if (cliParser.files.size() == 2)
    splitColonDelimitedList(cliParser.files[1], params.providesRefsSources);
}

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "");
  initCliParser();
  parseCmdLine(argc, argv);
  if (cliParser.wasKeyUsed("--help"))
    {
      printHelp();
      return 0;
    }
  if (cliParser.wasKeyUsed("--log"))
    std::cout << "log" << std::endl;
  if (cliParser.wasKeyUsed("--debug"))
    std::cout << "debug" << std::endl;
  //  initLogging("/tmp/ds-references.log", LOG_DEBUG);//FIXME:
  printLogo();
  try {
    params.readInfoFile(Directory::mixNameComponents(params.indexPath, REPO_INDEX_INFO_FILE));
    IndexReconstructionListener listener;
    IndexCore indexCore(listener);
    indexCore.fixReferences(params);
  }
  catch(const DeepsolverException& e)
    {
      std::cerr << PREFIX << e.getType() << " error:" << e.getMessage() << std::endl;
      return 1;
    }
  catch(std::bad_alloc)
    {
      std::cerr << PREFIX << "no enough free memory to complete operation" << std::endl;
      return 1;
    }
  return 0;
}
