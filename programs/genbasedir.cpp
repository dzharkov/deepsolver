
#include"depsolver.h"
#include"IndexCore.h"
#include"rpm/RpmException.h"

#define PREFIX "genbasedir:"

static RepoIndexParams params;

class ConsoleMessages: public AbstractConsoleMessages
{
public:
  ConsoleMessages() {}

public:
  std::ostream& msg()
  {
    return std::cout;
  }

  std::ostream& verboseMsg()
  {
    return std::cout;
  }
}; //class ConsoleMessages;

class WarningHandler: public AbstractWarningHandler
{
public:
  WarningHandler(std::ostream& s):
    m_stream(s) {}

  void onWarning(const std::string& message)
  {
    m_stream << PREFIX << "warning:" << message << std::endl;
  }

private:
  std::ostream& m_stream;
}; //class WarningHandler;

static void splitColonDelimitedList(const std::string& str, StringList& res)
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
    return RepoIndexParams::CompressionTypeNone;
  if (value == "gzip")
    return RepoIndexParams::CompressionTypeGzip;
  return -1;
}

char selectFormatType(const std::string& value)
{
  if (value == "binary")
    return RepoIndexParams::FormatTypeBinary;
  if (value == "text")
    return RepoIndexParams::FormatTypeText;
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
  params.topDir = ".";
  while(1)
    {
      const int p = getopt(argc, argv, "c:d:hu:rp:l");
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
	  params.provideFilteringByRefs = 1;
	  break;
	case 'd':
	  splitColonDelimitedList(optarg, params.takeRefsFromPackageDirs);
	  break;
	case 'p':
	  splitColonDelimitedList(optarg, params.provideFilterDirs);
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
	  /*TODO:not implemented - not accessible;
	case 'f':
	  params.formatType = selectFormatType(optarg);
	  if (params.formatType < 0)
	    {
	      std::cerr << PREFIX << "value \'" << optarg << "\' is not a valid format type" << std::endl;
	      return 0;
	    }
	  break;
	  */
	default:
	  assert(0);
	}; //switch(p);
    } //while(1);
  assert(optind <= argc);
  if (optind == argc)
    {
      std::cerr << PREFIX << "packages architecture is not specified" << std::endl;
	return 0;
    }
  if (optind + 2 < argc)
    {
      std::cerr << PREFIX << "too many command line arguments" << std::endl;
	return 0;
}
  params.arch = argv[optind];
  if (optind + 1 < argc)
    params.topDir = argv[optind + 1];
  if (!hasNonSpaces(params.arch))
    {
      std::cerr << PREFIX << "Required architecture cannot be an empty string" << std::endl;
      return 0;
    }
  if (!hasNonSpaces(params.topDir))
    {
      std::cerr << PREFIX << "Repository directory cannot be an empty string" << std::endl;
      return 0;
    }
  return 1;
}

void run()
{
  ConsoleMessages consoleMessages;
  WarningHandler warningHandler(std::cerr);
  IndexCore indexCore(consoleMessages, warningHandler);
  indexCore.build(params);
  consoleMessages.msg() << "Repository index was built successfully!" << std::endl;
}

int main(int argc, char* argv[])
{
  if (!parseCmdLine(argc, argv))
    return 1;
  initLogging("/tmp/depsolver.log", LOG_DEBUG);//FIXME:
  try {
    run();
  }
  catch(const DepsolverException& e)
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
