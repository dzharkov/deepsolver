

#include<assert.h>
#include<iostream>
#include<sys/types.h>
#include<unistd.h>
#include"sys/stat.h"
#include<fcntl.h>
#include <curl/curl.h>
#include"Curl.h"

static bool curlWasInitialized = 0;

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
  curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, acceptCurlData);
  curl_easy_setopt(handle, CURLOPT_FILE, &recipient);
  curl_easy_perform(handle);
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
  curl.fetch("http://deepsolver.altlinux.org/tech-assignment.pdf", recipient);
  curl.close();
  close(fd);
  return 0;
}
