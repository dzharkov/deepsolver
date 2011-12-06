
#include"depsolver.h"
#include"ZLibInterface.h"

#define TEST_BLOCK_SIZE 512
#define TEST_BLOCK_COUNT 1024

void zlibRegression()
{
  int origFd = open("orig.data", O_CREAT | O_TRUNC | O_WRONLY, 0666);
  assert(origFd > 0);
  int zlibFd = open("zlib.data", O_CREAT | O_TRUNC | O_WRONLY, 0666);
  assert(zlibFd > 0);

  ZLibCompressor compressor;
  for(size_t i = 0;i < TEST_BLOCK_COUNT;i++)
    {
      char buf[TEST_BLOCK_SIZE];
      fillRandomBuf(buf, sizeof(buf));
      write(origFd, buf, sizeof(buf));
      char zlibBuf[TEST_BLOCK_SIZE];
      const char* srcBegin = buf;
      const char *srcEnd = srcBegin + TEST_BLOCK_SIZE;
      char * destBegin;
      char * destEnd;
      while(1)
	{
	  std::cout << "step" << std::endl;
	  char * destBegin = zlibBuf;
	  char * destEnd = destBegin + TEST_BLOCK_SIZE;
	  char* origValue = destBegin;
	  compressor.filter(srcBegin, srcEnd, destBegin, destEnd, i == TEST_BLOCK_COUNT - 1);
	  const int has = static_cast<int>(destBegin - origValue);
	  std::cout << "has=" << has << std::endl;
	  if (has > 0)
	    write(zlibFd, zlibBuf, has);
	  if (destEnd != destBegin)
	    {
	      std::cout << "finish" << std::endl;
		break;
	    }
	}
      assert(srcBegin == srcEnd);
    }
  close(origFd);
  close(zlibFd);

  //Decompressing;
  std::cout << "Decompressing" << std::endl;
  ZLibDecompressor decompressor;
  origFd = open("zlib.data", O_RDONLY);
  assert(origFd > 0);
  zlibFd = open("back.data", O_CREAT | O_TRUNC | O_WRONLY, 0666);
  assert(zlibFd > 0);

  while(1)
    {
      char buf[TEST_BLOCK_SIZE];
      const ssize_t readCount = read(origFd, buf, sizeof(buf));
      assert(readCount >= 0);
      if (readCount == 0)
	break;

      char zlibBuf[TEST_BLOCK_SIZE];
      const char* srcBegin = buf;
      const char *srcEnd = srcBegin + readCount;
      char * destBegin;
      char * destEnd;
      while(1)
	{
	  std::cout << "Decompression step" << std::endl;
	  char * destBegin = zlibBuf;
	  char * destEnd = destBegin + TEST_BLOCK_SIZE;
	  char* origValue = destBegin;
	  decompressor.filter(srcBegin, srcEnd, destBegin, destEnd, readCount < sizeof(buf));//FIXME:eof!!!
	  const int has = static_cast<int>(destBegin - origValue);
	  std::cout << "has=" << has << std::endl;
	  if (has > 0)
	    write(zlibFd, zlibBuf, has);
	  if (destEnd != destBegin)
	    {
	      std::cout << "Decompression finish" << std::endl;
		break;
	    }
	}
	  assert(srcBegin == srcEnd);
    }
  close(origFd);
  close(zlibFd);
}

int main(int argc, char* argv[])
{
  zlibRegression();
  return 0;
}
