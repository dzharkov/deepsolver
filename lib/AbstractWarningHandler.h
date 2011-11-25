
#ifndef DEPSOLVER_ABSTRACT_WARNING_HANDLER_H
#define DEPSOLVER_ABSTRACT_WARNING_HANDLER_H

class AbstractWarningHandler 
{
public:
  virtual ~AbstractWarningHandler() {}

public:
  virtual void onWarning(const std::string& message) = 0;
}; //class AbstractWarningHandler;

#endif //DEPSOLVER_ABSTRACT_WARNING_HANDLER_H;
