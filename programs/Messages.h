
#ifndef DEEPSOLVER_MESSAGES_H
#define DEEPSOLVER_MESSAGES_H

class Messages
{
public:
  Messages(std::ostream& stream)
    : m_stream(stream) {}

  virtual ~Messages() {}

public:
  void onSystemError(const SystemException& e);
  void onConfigSyntaxError(const ConfigFileException);

private:
  std::ostream& m_stream;
}; //class Messages;

#endif //DEEPSOLVER_MESSAGES_H;
