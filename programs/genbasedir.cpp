

#include"basic-header.h"//FIXME:
#include"IndexCore.h"

#define PREFIX "genbasedir:"

static RepoIndexParams params;
static std::string topDir = ".", requiredArch;

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

void printHelp()
{
  printf("%s%s", PREFIX,
	 "The utility to build index data of package repository\n\n"
	 "Usage:\n"
	 "\tgenbasedir [OPTIONS] [ARCH [TOPDIR]]\n\n"
	 "Valid command line options are:\n"
	 "\t-h - print this help screen;\n"
	 "\t-c TYPE - choose compression type: none or gzip (default is gzip);\n"
	 "\t-f FORMAT - choose data format: binary or text (default is text);\n"
	 "\t-n NAME - select the repository name (optional).\n\n"
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

bool parseCmdLine(int argc, char* argv[])
{
  while(1)
    {
      const int p = getopt(argc, argv, "c:f:hn:");
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
	case 'c':
	  params.compressionType = selectCompressionType(optarg);
	  if (params.compressionType < 0)
	    {
	      std::cerr << PREFIX << "value \'" << optarg << "\' is not a valid compression type" << std::endl;
	      return 0;
	    }
	  break;
	case 'f':
	  params.formatType = selectFormatType(optarg);
	  if (params.formatType < 0)
	    {
	      std::cerr << PREFIX << "value \'" << optarg << "\' is not a valid format type" << std::endl;
	      return 0;
	    }
	  break;
	case 'n':
	  params.repoName = optarg;
	  break;
	default:
	  assert(0);
	}; //switch(p);
    } //while(1);
  assert(optind <= argc);
  if (optind == argc)
    {
      std::cerr << PREFIX << "packages architecture is not specified" << std::endl << std::endl;
	printHelp();
	return 0;
    }
  if (optind + 2 < argc)
    {
      std::cerr << PREFIX << "too many command line arguments" << std::endl << std::endl;
	printHelp();
	return 0;
}
  requiredArch = argv[optind];
  if (optind + 1 < argc)
    topDir = argv[optind + 1];
  if (!hasNonSpaces(requiredArch))
    {
      std::cout << PREFIX << "Required architecture cannot be an empty string" << std::endl;
      return 0;
    }
  if (!hasNonSpaces(topDir))
    {
      std::cout << PREFIX << "Repository directory cannot be an empty string" << std::endl;
      return 0;
    }
  return 1;
}

void run()
{
  WarningHandler warningHandler(std::cerr);
  IndexCore indexCore(warningHandler);
  indexCore.build(topDir, requiredArch, params);
}

int main(int argc, char* argv[])
{
  if (!parseCmdLine(argc, argv))
    return 1;
  try {
    run();
  }
  catch(const IndexCoreException& e)
    {
      std::cerr << PREFIX << "index error:" << e.getMessage() << std::endl;
      return 1;
    }
  catch(std::bad_alloc)
    {
      std::cerr << PREFIX << "no enough free memory to complete operation" << std::endl;
      return 1;
    }
  return 0;
}
