

#include"depsolver.h"
#include"MD5.h"

void writeFile(const std:;string& fileName, size_t len)
{
  File f;
  f.create(fileName);
  size_t written = 0;
  srand(len);
  while(written < len)
    {
      unsigned char buf[2048];
      const size_t count = len > written + sizeof(buf)?sizeof(buf):len - written;
      for(size_t i = 0;i < count;i++)
	buf[i] = (unsigned char)(rand() % 256);
      file.write(buf, count);
    }
}

int main(int argc, char* argv[])
{
  assert(argc == 3);
  std::istringstream s(argv[2]);
  size_t len = 0;
  if (!(s >> len))
    {
      std::cerr << "\'" << argv[2] << "\' is not a valid length"
	return 1;
    }
  writeFile(argv[1], len);
  return 0;
}
