
#ifndef FIXME_INDEX_CORE_EXCEPTION_H
#define FIXME_INDEX_CORE_EXCEPTION_H

//FIXME:basic exception;
class IndexCoreException 
{
public:
  IndexCoreException() {}
  IndexCoreException(const std::string& message):
    m_message(message) {}

  virtual ~IndexCoreException() {}

public:
  std::string getMessage() const
  {
    return m_message;
  }

private:
  const std::string m_message;
}; //class IndexCoreException;

#define INDEX_CORE_STOP(x) throw IndexCoreException(x)

#endif //FIXME_INDEX_CORE_EXCEPTION_H;
