
#ifndef DEEPSOLVER_MESSAGES_H
#define DEEPSOLVER_MESSAGES_H

#include"os/SystemException.h"
#include"ConfigCenter.h"
#include"utils/CurlInterface.h"

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

private:
  std::ostream& m_stream;
}; //class Messages;

#endif //DEEPSOLVER_MESSAGES_H;
