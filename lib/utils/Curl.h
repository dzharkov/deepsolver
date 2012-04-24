

#ifndef DEEPSOLVER_CURL_H
#define DEEPSOLVER_CURL_H

class CurlException //FIXME: : public DeepsolverException
{
public:
  CurlException(int code, const std::string& url, const std::string& text)
    : m_code(code), m_url(url), m_text(text) {}

  virtual ~CurlException() {}

public:
  int getCode() const
  {
    return m_code;
  }

  std::string getUrl() const
  {
    return m_url;
  }

  std::string getText() const
  {
    return m_text;
  }

  std::string getType() const
  {
    return "curl";
  }

  std::string getMessage() const
  {
    return m_url + ":" + m_text;
  }

private:
  const int m_code;
  const std::string m_url;
  const std::string m_text;
}; //class CurlException;

class AbstractCurlDataRecipient
{
public:
  virtual ~AbstractCurlDataRecipient() {}

public:
  virtual size_t onNewDataBlock(const void* buf, size_t bufSize) = 0;
}; //class AbstractCurlDataRecipient;

class AbstractCurlProgressListener
{
public:
  virtual ~AbstractCurlProgressListener() {}

public:
  virtual void onCurlProgress(size_t now, size_t total) = 0;
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

  void fetch(const std::string& url,
	     AbstractCurlDataRecipient& recipient,
	     AbstractCurlProgressListener& progressListener);

private:
  void* m_handle;

}; //class CurlInterface;

#endif //DEEPSOLVER_CURL_H;
