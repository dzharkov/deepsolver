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
#include"rpm/RpmException.h"

#define PREFIX "ds-patch:"

static RepoParams params;
static StringVector filesToAdd, filesToRemove;

class IndexConstructionListener: public AbstractIndexConstructionListener
{
public:
  IndexConstructionListener() {}
  virtual ~IndexConstructionListener() {}

public:
  void onReferenceCollecting(const std::string& path) {}
  void onPackageCollecting(const std::string& path) {}
  void onProvidesCleaning() {}

  void onChecksumWriting()
  {
    std::cout << "Writing checksum file" << std::endl;
  }

  void onChecksumVerifying() 
  {
    std::cout << "Verifying checksums" << std::endl;
  }

  void onPatchingFile(const std::string& fileName)
  {
    std::cout << "Patching file " << fileName << std::endl;
  }

  void onNoTwiceAdding(const std::string& fileName)
  {
    std::cerr << "WARNING! File \'" << fileName << "\' already included in index, no second adding" << std::endl;
  }
}; //class IndexConstructionListener;

void printLogo()
{
  std::cout << "ds-patch: utility to patch Deepsolver repository index" << std::endl;
    std::cout << "Version: " << PACKAGE_VERSION << std::endl;
  std::cout << std::endl;
}

void printHelp()
{
  printLogo();
  printf("%s", 
	 "Usage:\n"
	 "\tds-patch [--help] INDEX_DIR [--add FILE1 [FILE2 [...]]] [--del FILE1 [FILE2 [...]]]\n"
	 "Where:\n"
	 "\tINDEX_DIR - directory with indices to patch;\n"
	 "\tFILE1, FILE2... - files to add or delete; files to add must be mentioned by their absolute path, file to delete - just by file names.\n"
	 "\n"
	 "NOTE: New packages are added to index without any provides filtering, use ds-references utility for consequent provides filtering.\n");
}

bool parseCmdLine(int argc, char* argv[])
{
  for(int i = 1;i < argc;i++)
    {
      const std::string value = argv[i];
      if (value == "--help" || value == "-h")
	{
	  printHelp();
	  exit(EXIT_SUCCESS);
	}
    }
  if (argc < 2)
    {
      printHelp();
      exit(EXIT_SUCCESS);
    }
  if (argc == 2)
    {
      std::cerr << PREFIX << "No packages mentioned to add or remove" << std::endl;
      return 0;
    }
  params.indexPath = argv[1];
  int mode = 0;
  for(int i = 2;i < argc;i++)
    {
      const std::string value = argv[i];
      if (value == "--add")
	{
	  mode = 1;
	  continue;
	}
      if (value == "--del")
	{
	  mode = 2;
	  continue;
	}
      assert(mode >= 0 && mode <= 2);
      if (mode == 0)
	{
	  std::cerr << PREFIX << "Unknown parameter \'" << value << "\', if it is a file to add or remove you should use \'--add\' or \'--del\' key before" << std::endl;
	  return 0;
	}
      if (mode == 1)
	filesToAdd.push_back(value); else
	filesToRemove.push_back(value);
    }
  return 1;
}

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "");
  if (!parseCmdLine(argc, argv))
    return 1;
  //  initLogging("/tmp/ds-patch.log", LOG_DEBUG);//FIXME:
  printLogo();
  if (filesToAdd.empty() && filesToRemove.empty())
    {
      std::cout << "Nothing to add and nothing to remove!" << std::endl;
	return 0;
    }
  try {
    params.readInfoFile(Directory::mixNameComponents(params.indexPath, REPO_INDEX_INFO_FILE));
    IndexConstructionListener listener;
    IndexCore indexCore(listener);
    indexCore.rebuildIndex(params, filesToAdd, filesToRemove);
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
