
#include"basic-header.h"//FIXME:

#define IO_BUF_SIZE 2048

//Ensures block of required length is read completely or there is no more data (prevents from incomplete read() operation);
static ssize_t readBlock(int fd, void* buf, size_t bufSize)
{
  assert(buf);
  char* b = (char*)buf;
  size_t c = 0;
  while(c < bufSize)
    {
      ssize_t res = read(fd, &b[c], bufSize - c);
      if (res == -1)
	return -1;
      if (res == 0)
	break;
      assert(res > 0);
      c += (size_t)res;
    } //while();
  return (ssize_t)c;
}

//Ensures block of required length is write completely (prevents from incomplete write() operation);
static ssize_t writeBlock(int fd, const void* buf, size_t bufSize)
{
  assert(buf);
  char* b = (char*)buf;
  size_t c = 0;
  while(c < bufSize)
    {
      ssize_t res = write(fd, &b[c], bufSize - c);
      if (res == -1)
	return -1;
      assert(res >= 0);
      c += (size_t)res;
    } //while();
  assert(c == bufSize);
  return (ssize_t)c;
}

static ssize_t readBuffer(int fd, void* buf, size_t bufSize)
{
  assert(buf);
  char* b = (char*)buf;
  size_t c = 0;
  while(c < bufSize)
    {
      const size_t requiredSize = bufSize > c + IO_BUF_SIZE?IO_BUF_SIZE:(size_t)(bufSize - c);
      const ssize_t res = readBlock(fd, &b[c], requiredSize);
      if (res == -1)
	return -1;
      c += (size_t)res;
      if (res < (ssize_t)requiredSize)//Incomplete read() operation is impossible here;
	break;
    } //while();
  return (ssize_t)c;
}

static ssize_t writeBuffer(int fd, const void* buf, size_t bufSize)
{
  assert(buf);
  char* b = (char*)buf;
  size_t c = 0;
  while(c < bufSize)
    {
      const size_t requiredSize = bufSize > c + IO_BUF_SIZE?IO_BUF_SIZE:(size_t)(bufSize - c);
      const ssize_t res = writeBlock(fd, &b[c], requiredSize);
      if (res == -1)
	return -1;
      assert(res == (ssize_t)requiredSize);//Write operation can be completed or fail with -1 exit code;
      c += (size_t)res;
    } //while();
  assert(c == bufSize);
  return (ssize_t)c;
}

void File::open(const std::string fileName)
{
  assert(m_fd == -1);
  m_fd = ::open(fileName.c_str(), O_RDWR);
  if (m_fd < 0)
    {
      m_fd = -1;
      SYS_STOP("open(" + fileName + "\', O_RDWR");
    }
}

void File::openReadOnly(const std::string& fileName)
{
  assert(m_fd == -1);
  m_fd = ::open(fileName.c_str(), O_RDONLY);
  if (m_fd < 0)
    {
      m_fd = -1;
      SYS_STOP("open(" + fileName + "\', O_RDONLY");
    }
}

void File::create(const std::string& fileName)
{
  assert(m_fd == -1);
  m_fd = ::open(fileName.c_str(), O_RDWR | O_CREAT | O_TRUNC, NEW_FILE_MODE);
  if (m_fd < 0)
    {
      m_fd = -1;
      SYS_STOP("open(" + fileName + "\', O_RDONLY");
    }
}

void File::close()
{
  if (m_fd < 0)
    return;
  ::close(m_fd);
  m_fd = -1;
}

size_t File::read(void* buf, size_t bufSize)
{
  assert(m_fd >= 0);
  const ssize_t res = readBuffer(m_fd, buf, bufSize);
  if (res < 0)
    SYS_STOP("read()");
  return (size_t)res;
}

size_t File::write(const void* buf, size_t bufSize)
{
  assert(m_fd >= 0);
  const ssize_t res = writeBuffer(m_fd, buf, bufSize);
  if (res < 0)
    SYS_STOP("write()");
  return (size_t)res;
}
