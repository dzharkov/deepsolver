

#ifndef DEEPSOLVER_ABSTRACT_CONSOLE_MESSAGES_H
#define DEEPSOLVER_ABSTRACT_CONSOLE_MESSAGES_H

class AbstractConsoleMessages
{
public:
  virtual ~AbstractConsoleMessages() {}

public:
  virtual std::ostream& msg() = 0;
  virtual std::ostream& verboseMsg() = 0;
}; //class AbstractConsoleMessages;

#endif //DEEPSOLVER_ABSTRACT_CONSOLE_MESSAGES_H;
