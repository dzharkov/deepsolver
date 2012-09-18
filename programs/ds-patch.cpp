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

#define PREFIX "ds-patch:"

class IndexReconstructionListener: public AbstractIndexConstructionListener
{
public:
  IndexReconstructionListener() {}
  virtual ~IndexReconstructionListener() {}

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
  while (ending < params.size() && params[ending] != "--add" && params[ending] != "--del" && findKey(params[ending]) == (KeyVector::size_type)-1)
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

static RepoParams params;
static DsPatchCliParser cliParser;

void initCliParser()
{
  cliParser.addKeyDoubleName("-h", "--help", "print this help screen and exit");
  cliParser.addKey("--log", "print log to console instead of user progress information");
  cliParser.addKey("--debug", "relax filtering level for log output");
}

void printLogo()
{
  std::cout << "ds-patch: the utility to patch Deepsolver repository index" << std::endl;
    std::cout << "Version: " << PACKAGE_VERSION << std::endl;
  std::cout << std::endl;
}

void printHelp()
{
  printLogo();
  printf("%s", 
	 "Usage:\n"
	 "\tds-patch [-OPTIONS] INDEX_DIR [--add FILE1 [FILE2 [...]]] [--del FILE1 [FILE2 [...]]]\n"
	 "Where:\n"
	 "\tINDEX_DIR - directory with index to patch\n"
	 "\tFILE1, FILE2... - files to add or delete; files to add must be mentioned by their absolute path, file to delete - just by file names\n"
"\n"
	 "Valid command line options are:\n");
  cliParser.printHelp(std::cout);
  std::cout << "NOTE: New packages are added to index without any provides filtering, use ds-references utility for consequent provides filtering." << std::endl;
}

void parseCmdLine(int argc, char* argv[])
{
  cliParser.init(argc, argv);
  cliParser.parse();//FIXME:
  if (cliParser.wasKeyUsed("--help"))
    {
      printHelp();
      exit(EXIT_SUCCESS);
    }
  if (cliParser.filesToAdd.empty() && cliParser.filesToRemove.empty())
    {
      std::cout << "Nothing to add and nothing to remove!" << std::endl;
      exit(EXIT_SUCCESS);
    }
}

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "");
  initCliParser();
  parseCmdLine(argc, argv);
  initLogging(cliParser.wasKeyUsed("--debug")?LOG_DEBUG:LOG_INFO, cliParser.wasKeyUsed("--log"));
  return 0;//FIXME:
  try {
    if (!cliParser.wasKeyUsed("--log"))
      printLogo();
    params.readInfoFile(Directory::mixNameComponents(params.indexPath, REPO_INDEX_INFO_FILE));
    IndexReconstructionListener listener;
    IndexCore indexCore(listener);
    indexCore.rebuildIndex(params, cliParser.filesToAdd, cliParser.filesToRemove);
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
