

#ifndef DEEPSOLVER_CURL_H
#define DEEPSOLVER_CURL_H

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
