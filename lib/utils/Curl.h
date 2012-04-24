

#ifndef DEEPSOLVER_CURL_H
#define DEEPSOLVER_CURL_H

class CurlException //FIXME: : public DeepsolverException
{
public:
  CurlException(int code, const std::string& message)
    : m_code(code), m_message(message) {}

  virtual ~CurlException() {}

public:
  int getCode() const
  {
    return m_code;
  }

  std::string getType() const
  {
    return "curl";
  }

  std::string getMessage() const
  {
    return m_message;
  }

private:
  const int m_code;
  const std::string m_message;
}; //class CurlException;

class AbstractCurlDataRecipient
{
public:
  virtual ~AbstractCurlDataRecipient() {}

public:
  virtual size_t onNewDataBlock(const void* buf, size_t bufSize) = 0;
}; //class AbstractCurlDataRecipient;

class CurlInterface
{
public:
  CurlInterface()
    : m_handle(NULL)  {}

  virtual ~CurlInterface() 
  {
    close();
  }

public:
  void init();
  void close();
  void fetch(const std::string& url, AbstractCurlDataRecipient& recipient);

private:
  void* m_handle;

}; //class CurlInterface;

#endif //DEEPSOLVER_CURL_H;
