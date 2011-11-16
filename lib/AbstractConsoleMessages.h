

#ifndef FIXME_ABSTRACT_CONSOLE_MESSAGES_H
#define FIXME_ABSTRACT_CONSOLE_MESSAGES_H

class AbstractConsoleMessages
{
public:
  virtual void msg(const std::string& text) = 0;
  virtual void msgNewLine(const std::string& text) = 0;
}; //class AbstractConsoleMessages;

#endif //FIXME_ABSTRACT_CONSOLE_MESSAGES_H;
