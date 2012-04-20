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
  //FIXME:
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
