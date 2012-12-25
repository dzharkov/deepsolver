/*
 * This file now provides only support for English language messages but
 * generally multilingual support implies.
 */

#include"deepsolver.h"
#include"Messages.h"

std::string messagesProgramName;

struct ConfigSyntaxErrorMessage 
{
  int code;
  const char* message;
};

const ConfigSyntaxErrorMessage configSyntaxErrorMessages[] = {
  {ConfigErrorSectionInvalidType, "section header may have only the first level name used with argument"},
  {ConfigErrorSectionWaitingOpenBracket, "expecting open bracket"},
  {ConfigErrorSectionWaitingName, "expecting section name"},
  {ConfigErrorSectionInvalidNameChar, "invalid character in section name or unexpected end"},
  {ConfigErrorSectionWaitingCloseBracketOrArg, "expecting close bracket or section argument"},
  {ConfigErrorSectionUnexpectedArgEnd, "unexpected section argument end"},
  {ConfigErrorSectionWaitingCloseBracket, "expecting close bracket"},
  {ConfigErrorValueWaitingName, "expecting parameter name"},
  {ConfigErrorValueInvalidNameChar, "invalid parameter name character or unexpected end"},
  {ConfigErrorValueWaitingAssignOrNewName, "expecting assignment or new level name"},
  {ConfigErrorValueWaitingNewName, "expecting new level name"},
  {ConfigErrorValueUnexpectedValueEnd, "unexpected value end"},
  {-1, NULL}
};

static std::string getConfigSyntaxErrorText(int code)
{
  size_t i = 0;
  while (configSyntaxErrorMessages[i].code != code && configSyntaxErrorMessages[i].code >= 0 && configSyntaxErrorMessages[i].message != NULL)
    i++;
  assert(configSyntaxErrorMessages[i].code >= 0 && configSyntaxErrorMessages[i].message != NULL);
  return configSyntaxErrorMessages[i].message;
}

void Messages::onSystemError(const SystemException& e)
{
  m_stream << std::endl;

  m_stream << "System error were occurred during last operation. That may be caused" << std::endl;
  m_stream << "by improper Deepsolver installation or by damaged environment." << std::endl;
  m_stream << "Here is problem details:" << std::endl;
  m_stream << std::endl;
  m_stream << e.getMessage() << std::endl;
  m_stream << std::endl;
}

void Messages::onConfigSyntaxError(const ConfigFileException& e)
{
  m_stream << "There is an error in your configuration file. Please, consult your" << std::endl;
  m_stream << "system administrator for problem resolving. Details:" << std::endl;
  m_stream << std::endl;
  m_stream << "ERROR:" << getConfigSyntaxErrorText(e.getCode()) << std::endl;
  m_stream << std::endl;
  std::ostringstream ss;
  ss << e.getFileName() << ":" << e.getLineNumber() << ":";
  const size_t pos = ss.str().length() + e.getPos();//FIXME:UTF-8 character make this value incorrect;
  m_stream << ss.str() << e.getLine() << std::endl;
  for(size_t i = 0;i < pos;i++)
    m_stream << " ";
  m_stream << "^" << std::endl;
}

void Messages::onConfigError(const ConfigException& e)
{
  m_stream << "There is an error in your configuration file. Please, consult your" << std::endl;
  m_stream << "system administrator for problem resolving. Details listed below:" << std::endl;
  m_stream << std::endl;
  if (e.getLineNumber() > 0)
    m_stream << e.getFileName() << ":" << e.getLineNumber() << ":";
  switch(e.getCode())
    {
    case ConfigException::UnknownParam:
      m_stream << "unknown parameter: " << e.getArg() << std::endl;
  break;
    case ConfigException::ValueCannotBeEmpty:
      m_stream << "value of parameter \'" << e.getArg() << "\' cannot be empty" << std::endl;
  break;
    case ConfigException::AddingNotPermitted:
      m_stream << "adding not permitted" << std::endl;
  break;
    case ConfigException::InvalidBooleanValue:
      m_stream << "invalid boolean value" << std::endl;
  break;
    case ConfigException::InvalidUintValue:
      m_stream << "invalid unsigned integer value" << std::endl;
  break;
    default:
      assert(0);
      return;
    } //switch(e.getCode());
  if (e.getLineNumber() > 0)
    {
      m_stream << std::endl;
      m_stream << "Invalid line content:" << std::endl;
    m_stream << e.getLine() << std::endl;
    }
}

void Messages::onCurlError(const CurlException& e)
{
  m_stream << std::endl;
  m_stream << "Your network connection is experiencing difficulties or you have the" << std::endl;
  m_stream << "error in your configuration files. Please, consult your system" << std::endl;
  m_stream << "administrator. Problem detailes:" << std::endl;
  m_stream << std::endl;
  m_stream << "URL: " << e.getUrl() << std::endl;
  m_stream << "ERROR: " << e.getText() << std::endl;
  m_stream << std::endl;
}

void Messages::onRpmError(const RpmException& e)
{
  m_stream << std::endl;
  m_stream << "RpmError:" << e.getMessage() << std::endl;
}

void Messages::onOperationError(const OperationException& e)
{
  m_stream << std::endl;
  m_stream << "The requested operation cannot be properly performed due to unexpected errors." << std::endl;
  switch(e.getCode())
    {
    case OperationException::InvalidInfoFile:
      m_stream << "the information file for one or more repositories contains incorrect" << std::endl;
      m_stream << "data or damaged. Usually it means an invalid URL of remote repository is used" << std::endl;
      m_stream << "or repository provider is experiencing technical problems. Please, be" << std::endl;
      m_stream << "sure your configuration data is correct and try again later." << std::endl;
      m_stream << "Here is the URL of the incorrect info file:" << std::endl;
      m_stream << std::endl;
      m_stream << e.getArg() << std::endl;
      m_stream << std::endl;
      break;
    case OperationException::InvalidChecksumData:
      m_stream << "One of the attached repositories has corrupted checksum file and it" << std::endl;
      m_stream << "cannot be properly parsed. Usually it means you have an error in your" << std::endl;
      m_stream << "configuration data or repository provider is experiencing technical" << std::endl;
      m_stream << "problems. Without checksum data Deepsolver is unable to be sure" << std::endl;
      m_stream << "the fetched files are not broken. Please, consult your system" << std::endl;
      m_stream << "administrator or try again later. Here is the URL of the invalid" << std::endl;
      m_stream << "checksum file:" << std::endl;
      m_stream << std::endl;
      m_stream << e.getArg() << std::endl;
      m_stream << std::endl;
      break;
    case OperationException::BrokenIndexFile:
      m_stream << "one or more files in repository index is broken. Since basic repository data" << std::endl;
      m_stream << "is correct very likely it means the repository provider is experiencing technical" << std::endl;
      m_stream << "problems. Here is the URL of the corrupted file:" << std::endl;
      m_stream << std::endl;
      m_stream << e.getArg() << std::endl;
      m_stream << std::endl;
      break;
    case OperationException::InternalIOProblem:
      m_stream << "The internal program error was occurred. The most likely that means" << std::endl;
      m_stream << "the Deepsolver was not properly installed or you are running broken" << std::endl;
      m_stream << "system. This type of error may not be caused by corrupted external" << std::endl;
      m_stream << "data or an invalid user input. The only reason is incorrect execution" << std::endl;
      m_stream << "environment. Please, contact your system administrator." << std::endl;
      break;
    default:
      assert(0);
    } //switch(e.getCode());
}

void Messages::onTaskError(const TaskException& e)
{
  m_stream << std::endl;
  switch(e.getCode())
    {
    case TaskException::UnknownPackageName:
      m_stream << "You have requested an unknown package. Provided name \"" << e.getArg() << "\" is not present" << std::endl;
      m_stream << "neither in available repositories nor in the system." << std::endl;
      return;
    case TaskException::UnsolvableSat:
      m_stream << "The package \"" << e.getArg() << "\" was considered to be installed and removed simultaneously. That can" << std::endl;
      m_stream << "be caused by problems in the package dependencies set or too difficult" << std::endl;
	m_stream << "user request. To resolve this situation you can simplify your task by" << std::endl;
	m_stream << "listing less packages if there are several." << std::endl;
	return;
    case TaskException::NoSatSolution:
      m_stream << "Deepsolver task processor is unable to find a valid solution. That may" << std::endl;
      m_stream << "be caused by problems in the package dependencies set or too difficult" << std::endl;
      m_stream << "user request. To resolve this situation you can simplify your task by" << std::endl;
      m_stream << "listing less packages if there are several." << std::endl;
      return;
    case TaskException::Unmet:
      m_stream << "The require entry \"" << e.getArg() << "\" causes an unmet. That means there are no packages" << std::endl;
      m_stream << "neither in the available repositories nor in the system suitable to" << std::endl;
      m_stream << "satisfy this requirement. Probably there are problems in package set" << std::endl;
      m_stream << "you are working with. Please, contact your repositories administrator." << std::endl;
      return;
    case TaskException::NoRequestedPackage:
      m_stream << "There are no suitable packages to match your task request \"" << e.getArg() << "\". Please," << std::endl;
      m_stream << "consider another expression." << std::endl;
	return;
    default:
      assert(0);
  };
}

//Command line errors;

void Messages::onMissedProgramName() const
{
  m_stream << messagesProgramName << ":The command line doesn\'t contain program name" << std::endl;
}

void Messages::onMissedCommandLineArgument(const std::string& arg) const
{
  m_stream << messagesProgramName << ":The command line argument \'" << arg << "\' requires additional parameter" << std::endl;
}

// User input errors;

void Messages::onNoPackagesMentionedError() const
{
  m_stream << messagesProgramName << ":No packages mentioned" << std::endl;
}

// ds-update;

void Messages::dsUpdateLogo() const
{
  m_stream << "ds-update: the utility to fetch repository headers" << std::endl;
  m_stream << "Version: " << PACKAGE_VERSION << std::endl;
  m_stream << std::endl;
}

void Messages::dsUpdateInitCliParser(CliParser& cliParser) const
{
  cliParser.addKeyDoubleName("-h", "--help", "print this help screen and exit");
  cliParser.addKey("--log", "print log to console instead of user progress information");
  cliParser.addKey("--debug", "relax filtering level for log output");
}

void Messages::dsUpdateHelp(const CliParser& cliParser) const
{
  dsUpdateLogo();
  m_stream << "Usage: ds-update [OPTIONS]" << std::endl;
  m_stream << std::endl;
  m_stream << "Valid command line options are:" << std::endl;
  cliParser.printHelp(m_stream);
}

void Messages::introduceRepoSet(const ConfigCenter& conf) const
{
  for(ConfRepoVector::size_type i = 0;i < conf.root().repo.size();i++)
    {
      const ConfRepo& repo = conf.root().repo[i];
      std::cout << "Repo: " << repo.name << " (" << repo.url << ")" << std::endl;
      std::cout << "Arch:";
      for(StringVector::size_type k = 0;k < repo.arch.size();k++)
	std::cout << " " << repo.arch[k];
      std::cout << std::endl;
      std::cout << "Components:";
      for(StringVector::size_type k = 0;k < repo.components.size();k++)
	std::cout << " " << repo.components[k];
      std::cout << std::endl;
      std::cout << std::endl;
    }
}

// ds-install;

void Messages::dsInstallLogo() const
{
  m_stream << "ds-install: the Deepsolver utility for package installation" << std::endl;
  m_stream << "Version: " << PACKAGE_VERSION << std::endl;
  m_stream << std::endl;
}

void Messages::dsInstallInitCliParser(CliParser& cliParser) const
{
  cliParser.addKeyDoubleName("-s", "--sat", "print SAT equation and do not touch any packages");
  cliParser.addKeyDoubleName("-h", "--help", "print this help screen and exit");
  cliParser.addKey("--log", "print log to console instead of user progress information");
  cliParser.addKey("--debug", "relax filtering level for log output");
}

void Messages::dsInstallHelp(const CliParser& cliParser) const
{
  dsInstallLogo();
  m_stream << "Usage: ds-install [OPTIONS] [PKG1 [(<|<=|=|>=|>) VERSION] [...]]" << std::endl;
  m_stream << std::endl;
  m_stream << "Valid command line options are:" << std::endl;
  cliParser.printHelp(m_stream);
}

// ds-remove;

void Messages::dsRemoveLogo() const
{
  m_stream << "ds-remove: the Deepsolver utility for package removing" << std::endl;
  m_stream << "Version: " << PACKAGE_VERSION << std::endl;
  m_stream << std::endl;
}

void Messages::dsRemoveInitCliParser(CliParser& cliParser) const
{
  cliParser.addKeyDoubleName("-s", "--sat", "print SAT equation and do not touch any packages");
  cliParser.addKeyDoubleName("-h", "--help", "print this help screen and exit");
  cliParser.addKey("--log", "print log to console instead of user progress information");
  cliParser.addKey("--debug", "relax filtering level for log output");
}

void Messages::dsRemoveHelp(const CliParser& cliParser) const
{
  dsRemoveLogo();
  m_stream << "Usage: ds-remove [OPTIONS] [PKG1 [PKG2 [...]]]" << std::endl;
  m_stream << std::endl;
  m_stream << "Valid command line options are:" << std::endl;
  cliParser.printHelp(m_stream);
}
