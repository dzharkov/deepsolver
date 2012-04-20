/*
 * This file now provides only support for English language messages but
 * generally multilingual support implies.
 */

#include"deepsolver.h"
#include"Messages.h"

#define PREFIX "deepsolver:"

struct ConfigSyntaxErrorMessage 
{
  int code;
  const char* message;
};

const ConfigSyntaxErrorMessage configSyntaxErrorMessages[] = {
  {ConfigErrorSectionInvalidType, ""},
  {ConfigErrorSectionWaitingOpenBracket, ""},
  {ConfigErrorSectionWaitingName, ""},
  {ConfigErrorSectionInvalidNameChar, ""},
  {ConfigErrorSectionWaitingCloseBracketOrArg, ""},
  {ConfigErrorSectionInvalidArgChar, ""},
  {ConfigErrorSectionWaitingCloseBracket, ""},
  {ConfigErrorValueWaitingName, ""},
  {ConfigErrorValueInvalidNameChar, ""},
  {ConfigErrorValueWaitingAssignOrNewName, ""},
  {ConfigErrorValueWaitingNewName, ""},
  {ConfigErrorValueInvalidValueChar, ""},
  {-1, NULL}
};

static std::string getConfigSyntaxErrorText(int code)
{
  return "FIXME";//FIXME:
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
