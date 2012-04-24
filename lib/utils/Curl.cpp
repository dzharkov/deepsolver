

#include<assert.h>
#include<iostream>
#include<sys/types.h>
#include<unistd.h>
#include"sys/stat.h"
#include<fcntl.h>
#include <curl/curl.h>
#include"Curl.h"

static bool curlWasInitialized = 0;

static int curlProgress(void* p,
			double dlTotal,
			double dlNow,
			double ulTotal,
			double ulNow)
{
  const size_t now = (size_t)dlNow;
  const size_t total = (size_t)dlTotal;
  std::cout << now << "/" << total << std::endl;
  return 0;
}

static size_t acceptCurlData(void* buf,
			     size_t size,
			     size_t nMemB,
			     void* param)
{
  assert(param != NULL);
  assert(buf != NULL);
  AbstractCurlDataRecipient* recipient = (AbstractCurlDataRecipient*)param;
  return recipient->onNewDataBlock(buf, size * nMemB);
}

void curlInitialize()
{
  if (curlWasInitialized)
    return;
  curl_global_init(CURL_GLOBAL_ALL);
  curlWasInitialized = 1;
}

void CurlInterface::init()
{
  CURL* handle = curl_easy_init();
  assert(handle != NULL);
  m_handle = handle;
}

void CurlInterface::close()
{
  if (m_handle == NULL)
    return;
  CURL* handle = (CURL*)m_handle;
  assert(handle != NULL);
  curl_easy_cleanup(handle);
  m_handle = NULL;
}

void CurlInterface::fetch(const std::string& url, AbstractCurlDataRecipient& recipient)
{
  assert(!url.empty());
  CURL* handle = (CURL*)m_handle;
  assert(handle != NULL);
  curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
  //Uncomment the following line if you want to see debug messages from libcurl on your console;
  //curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L);

  curl_easy_setopt(handle, CURLOPT_PROGRESSFUNCTION, curlProgress);
  curl_easy_setopt(handle, CURLOPT_PROGRESSDATA, &recipient);//FIXME:


  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, acceptCurlData);
  curl_easy_setopt(handle, CURLOPT_FILE, &recipient);
  const CURLcode res = curl_easy_perform(handle);
  if (res)
    throw CurlException(res, curl_easy_strerror(res));
}

class DataRecipient: public AbstractCurlDataRecipient
{
public:
  DataRecipient(int fd)
    : m_fd(fd) {}

  virtual ~DataRecipient() {}

public:
  size_t onNewDataBlock(const void* buf, size_t bufSize)
  {
    write(m_fd, buf, bufSize);
    return bufSize;
  }

private:
  int m_fd;
};

int main()
{
  int fd = open("/tmp/proba.bin", O_WRONLY | O_CREAT);
  assert(fd >= 0);
  DataRecipient recipient(fd);
  curlInitialize();
  CurlInterface curl;
  curl.init();
  try {
    //    curl.fetch("http://deepsolver.altlinux.org/tech-assignment.iso", recipient);

  }
  catch(const CurlException& e)
    {
      std::cerr << "curl:" << e.getMessage() << std::endl;
    }
  curl.close();
  close(fd);
  return 0;
}
