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

#define PREFIX "ds-repo:"

static RepoParams params;

class IndexConstructionListener: public AbstractIndexConstructionListener
{
public:
  IndexConstructionListener() {}
  virtual ~IndexConstructionListener() {}

public:
  void onReferenceCollecting(const std::string& path)
  {
    std::cout << "Collecting requires/conflicts in " << path << std::endl;
  }

  void onPackageCollecting(const std::string& path)
  {
    std::cout << "Reading packages in " << path << std::endl;
  }

  void onProvidesCleaning()
  {
    std::cout << "Provides filtering" << std::endl;
  }
}; //class IndexConstructionListener;

static void splitColonDelimitedList(const std::string& str, StringVector& res)
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

static bool splitUserParam(const std::string& str, std::string& name, std::string& value)
{
  name.erase();
  value.erase();
  bool wasEquals = 0;
  for(std::string::size_type i = 0;i < str.length();i++)
    {
      if (!wasEquals && str[i] == '=')
	{
	  wasEquals = 1;
	  continue;
	}
      if (!wasEquals)
	name += str[i]; else
	value += str[i];
    }
  return wasEquals;
}

static void printHelp()
{
  printf("%s%s", PREFIX,
	 "The utility to build index data of package repository\n\n"
	 "Usage:\n"
	 "\tgenbasedir [OPTIONS] [ARCH [TOPDIR]]\n\n"
	 "Valid command line options are:\n"
	 "\t-h - print this help screen;\n"
	 "\t-c TYPE - choose compression type: none or gzip (default is gzip);\n"
	 "\t-e FILENAME - ignore require entries mentioned in FILENAME;\n"
	 "\t-d DIRLIST - add colon-delimited list of directories to take references from for provides filtering (in addition to \'-r\' if it is used);\n"
	 "\t-l - include change log into binary and source indices;\n"
	 //TODO:	 "\t-f FORMAT - choose data format: binary or text (default is text);\n"
	 "\t-p DIRLIST - enable provides filtering by colon-delimited list of directories;\n"
	 "\t-r - enable provides filtering by used requires/conflicts (recommended) (see also \'-d\' option);\n"
	 "\t-u NAME=VALUE - add a user defined parameter to repository index information file.\n\n"
	 "If directory is not specified current directory is used to search packages of repository.\n"
	 );
}

char selectCompressionType(const std::string& value)
{
  if (value == "none")
    return RepoParams::CompressionTypeNone;
  if (value == "gzip")
    return RepoParams::CompressionTypeGzip;
  return -1;
}

char selectFormatType(const std::string& value)
{
  if (value == "binary")
    return RepoParams::FormatTypeBinary;
  if (value == "text")
    return RepoParams::FormatTypeText;
  return -1;
}

static bool processUserParam(const std::string& s)
{
  //FIXME:do not allow using of reserved parameters;
  std::string name, value;
  if (!splitUserParam(s, name, value))
    return 0;
  for(std::string::size_type i = 0;i < name.length();i++)
    if (s[i] == '#' || s[i] == '\\' || BLANK_CHAR(s[i]))
      return 0;
  params.userParams.insert(StringToStringMap::value_type(name, value));
  return 1;
}

static bool parseCmdLine(int argc, char* argv[])
{
  while(1)
    {
      const int p = getopt(argc, argv, "c:d:e:hu:rp:l");
      if (p == -1)
	break;
      if (p == '?')
	return 0;
      switch (p)
	{
	case 'h':
	  printHelp();
	  exit(EXIT_SUCCESS);
	  break;
	case 'l':
	  params.changeLogSources = 1;
	  params.changeLogBinary = 1;
	  break;
	case 'r':
	  params.filterProvidesByRefs = 1;
	  break;
	case 'd':
	  splitColonDelimitedList(optarg, params.providesRefsSources);
	  break;
	case 'e':
	  //FIXME:	  params.excludeRequiresFile = optarg;
	  break;
	case 'p':
	  splitColonDelimitedList(optarg, params.filterProvidesByDirs);
	  break;
	case 'u':
	  if (!processUserParam(optarg))
	    {
	      std::cerr << PREFIX << "\'" << optarg << "\' is not a valid user-defined parameter for index information file (probably missed \'=\' sign)" << std::endl;
return 0;
	    }
	  break;
	case 'c':
	  params.compressionType = selectCompressionType(optarg);
	  if (params.compressionType < 0)
	    {
	      std::cerr << PREFIX << "value \'" << optarg << "\' is not a valid compression type" << std::endl;
	      return 0;
	    }
	  break;
	default:
	  assert(0);
	}; //switch(p);
    } //while(1);
  StringVector values;
  for(int i = optind;i < argc;i++)
    values.push_back(argv[i]);
  if (values.empty())
    {
      std::cerr << PREFIX << "Index directory is not mentioned; try \'--help\' for usage information" << std::endl;
	return 0;
    }
  params.indexPath = values[0];
  if (values.size() == 1)
    {
      params.pkgSources.push_back(".");
      return 1;
    }
  for(StringVector::size_type i = 1;i < values.size();i++)
    params.pkgSources.push_back(values[i]);
  return 1;
}

void run()
{
  IndexConstructionListener listener;
  IndexCore indexCore(listener);
  indexCore.buildIndex(params);
}

int main(int argc, char* argv[])
{
  if (!parseCmdLine(argc, argv))
    return 1;
  initLogging("/tmp/genbasedir.log", LOG_DEBUG);//FIXME:
  try {
    run();
  }
  catch(const DeepsolverException& e)
    {
      std::cerr << std::endl;
      std::cerr << PREFIX << e.getType() << " error:" << e.getMessage() << std::endl;
      return 1;
    }
  catch(std::bad_alloc)
    {
      std::cerr << std::endl;
      std::cerr << PREFIX << "no enough free memory to complete operation" << std::endl;
      return 1;
    }
  return 0;
}
