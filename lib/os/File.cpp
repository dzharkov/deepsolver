
#define IO_BUF_SIZE 2048

ssize_t readBlock(int fd, void* buf, size_t bufSize)
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

ssize_t writeBlock(int fd, const void* buf, size_t bufSize)
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

ssize_t readBuffer(int fd, void* buf, size_t bufSize)
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
      if (res < (ssize_t)requiredSize)
	break;
    } //while();
  return (ssize_t)c;
}

ssize_t writeBuffer(int fd, const void* buf, size_t bufSize)
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
      assert(res == (ssize_t)requiredSize);
      c += (size_t)res;
    } //while();
  assert(c == bufSize);
  return (ssize_t)c;
}

