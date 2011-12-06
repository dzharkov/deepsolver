
#ifndef DEPSOLVER_RPM_EXCEPTION_H
#define DEPSOLVER_RPM_EXCEPTION_H

class RpmException: public DepsolverException
{
public:
  RpmException() {}
  RpmException(const std::string& message)
    : m_message(message
) {}

      virtual ~RpmException() {}

public:
  std::string getType() const
  {
    return "rpm";
  }

  std::string getMessage() const
  {
    return m_message;
  }

private:
  std::string m_message;
}; //class RpmException;

#define RPM_STOP(x) throw RpmException(x)

#endif //DEPSOLVER_RPM_EXCEPTION_H;
