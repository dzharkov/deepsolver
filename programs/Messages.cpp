/*
 * This file now provides only support for English language messages but
 * generally multilingual support implies.
 */

#include"deepsolver.h"
#include"Messages.h"

#define PREFIX "ds:"

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
  m_stream << "Some errors were occurred during the last operation:" << std::endl;
  m_stream << "system call failed, probably the utility was not properly installed." << std::endl;
    m_stream << std::endl;
    m_stream << e.getMessage() << std::endl;
}

void Messages::onConfigSyntaxError(const ConfigFileException& e)
{
  m_stream << PREFIX << "configuration file syntax error:" << getConfigSyntaxErrorText(e.getCode()) << std::endl;
  std::ostringstream ss;
  ss << e.getFileName() << "(" << e.getLineNumber() << "):";
  const size_t pos = ss.str().length() + e.getPos();//FIXME:UTF-8 character make this value incorrect;
  m_stream << ss.str() << e.getLine() << std::endl;
  for(size_t i = 0;i < pos;i++)
    m_stream << " ";
  m_stream << "^" << std::endl;
}

void Messages::onConfigError(const ConfigException& e)
{
  m_stream << PREFIX << "configuration file error:";
  switch(e.getCode())
    {
    case ConfigErrorUnknownParam:
      m_stream << "unknown parameter: " << e.getArg() << std::endl;
  break;
    case ConfigErrorIncompletePath:
      m_stream << "incomplete path: " << e.getArg() << std::endl;
  break;
    case ConfigErrorValueCannotBeEmpty:
      m_stream << "value of parameter \'" << e.getArg() << "\' cannot be empty" << std::endl;
  break;
    case ConfigErrorAddingNotPermitted:
      m_stream << "adding not permitted" << std::endl;
  break;
    default:
      assert(0);
      return;
    } //switch(e.getCode());
  if (e.getLineNumber() > 0)
    m_stream << e.getFileName() << "(" << e.getLineNumber() << "):" << e.getLine() << std::endl;
}

void Messages::onCurlError(const CurlException& e)
{
  m_stream << PREFIX << e.getUrl() << ":curl error " << e.getCode() << ":" << e.getText() << std::endl;
}

void Messages::onOperationError(const OperationException& e)
{
  m_stream << std::endl;
  m_stream << "Some errors were occurred during the last operation:" << std::endl;
  switch(e.getCode())
    {
    case OperationErrorInvalidInfoFile:
      //Maybe it is good idea to write also the name of repository caused the problem; 
      m_stream << "the information file for one or more repositories contains incorrect" << std::endl;
      m_stream << "data. Usually it means the invalid URL of remote repository was used" << std::endl;
      m_stream << "or repository provider is experiencing technical problems. Please, be" << std::endl;
      m_stream << "sure your configuration data is correct and try again later!" << std::endl;
      break;
      //FIXME:OperationErrorInvalidChecksum;
    default:
      assert(0);
    } //switch(e.getCode());
}
