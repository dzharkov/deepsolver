
#ifndef DEEPSOLVER_ABSTRACT_WARNING_HANDLER_H
#define DEEPSOLVER_ABSTRACT_WARNING_HANDLER_H

class AbstractWarningHandler 
{
public:
  virtual ~AbstractWarningHandler() {}

public:
  virtual void onWarning(const std::string& message) = 0;
}; //class AbstractWarningHandler;

#endif //DEEPSOLVER_ABSTRACT_WARNING_HANDLER_H;
