
#ifndef DEEPSOLVER_MESSAGES_H
#define DEEPSOLVER_MESSAGES_H

#include"ConfigCenter.h"
#include"utils/CurlInterface.h"
#include"OperationCore.h"
#include"CliParser.h"

extern std::string messagesProgramName;

class Messages
{
public:
  Messages(std::ostream& stream)
    : m_stream(stream) {}

  virtual ~Messages() {}

public:
  void onSystemError(const SystemException& e);
  void onConfigSyntaxError(const ConfigFileException& e);
  void onConfigError(const ConfigException& e);
  void onCurlError(const CurlException& e);
  void onRpmError(const RpmException& e);
  void onOperationError(const OperationException& e);
  void onTaskError(const TaskException& e);

  //Command line errors;
  void onMissedProgramName() const;
  void onMissedCommandLineArgument(const std::string& arg) const;

  //User input errors;
  void onNoPackagesMentionedError() const;

  //ds-update;
  void dsUpdateLogo() const;
  void dsUpdateInitCliParser(CliParser& cliParser) const;
  void dsUpdateHelp(const CliParser& cliParser) const;
  void introduceRepoSet(const ConfigCenter& conf) const;

  //ds-install;
  void dsInstallLogo() const;
  void dsInstallInitCliParser(CliParser& cliParser) const;
  void dsInstallHelp(const CliParser& cliParser) const;

  //ds-remove;
  void dsRemoveLogo() const;
  void dsRemoveInitCliParser(CliParser& cliParser) const;
  void dsRemoveHelp(const CliParser& cliParser) const;

private:
  std::ostream& m_stream;
}; //class Messages;

#endif //DEEPSOLVER_MESSAGES_H;
