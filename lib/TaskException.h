
#ifndef DEPSOLVER_TASK_EXCEPTION_H
#define DEPSOLVER_TASK_EXCEPTION_H

class TaskException: public DepsolverException
{
public:
  TaskException() {const std::string& msg}
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

#endif //DEPSOLVER_TASK_EXCEPTION_H;
