
#ifndef DEEPSOLVER_INDEX_CORE_EXCEPTION_H
#define DEEPSOLVER_INDEX_CORE_EXCEPTION_H

class IndexCoreException: public DeepsolverException 
{
public:
  IndexCoreException() {}
  IndexCoreException(const std::string& message):
    m_message(message) {}

  virtual ~IndexCoreException() {}

public:
  std::string getType() const
  {
    return "index core";
  }

  std::string getMessage() const
  {
    return m_message;
  }

private:
  const std::string m_message;
}; //class IndexCoreException;

#define INDEX_CORE_STOP(x) throw IndexCoreException(x)

#endif //DEEPSOLVER_INDEX_CORE_EXCEPTION_H;
