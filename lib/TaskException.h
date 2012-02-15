
#ifndef DEEPSOLVER_TASK_EXCEPTION_H
#define DEEPSOLVER_TASK_EXCEPTION_H

//This class must have i18n support in the future;
class TaskException: public DeepsolverException
{
public:
  TaskException(const std::string& msg)
    : m_message(msg) {}

  virtual ~TaskException() {}

public:
  std::string getType() const
  {
    return "task";
  }

  std::string getMessage() const
  {
    return m_message;
  }

private:
  std::string m_message;
}; //class TaskException;

#endif //DEEPSOLVER_TASK_EXCEPTION_H;
