

#include"depsolver.h"
#include"MD5.h"

void writeFile(const std::string& fileName, size_t len)
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
      f.write(buf, count);
      written += count;
    }
}

int main(int argc, char* argv[])
{
  assert(argc == 3);
  std::istringstream s(argv[2]);
  size_t len = 0;
  if (!(s >> len))
    {
      std::cerr << "\'" << argv[2] << "\' is not a valid length" << std::endl;
	return 1;
    }
  writeFile(argv[1], len);
  MD5 md5;
  md5.init();
  md5.updateFromFile(argv[1]);
  std::cout << md5.commit(argv[1]) << std::endl;
  return 0;
}
