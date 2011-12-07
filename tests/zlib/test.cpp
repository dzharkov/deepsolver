
#include"depsolver.h"
#include"ZLibInterface.h"

#define TEST_BLOCK_SIZE 512
#define TEST_BLOCK_COUNT 1024

#define ORIG_FILE_NAME "orig.data"
#define COMPRESSED_FILE_NAME "compressed.data"
#define DECOMPRESSED_FILE_NAME "decompressed.data"

void fillRandomBuf(void* buf, size_t len)
{
  assert(buf);
  unsigned char* c = static_cast<unsigned char*>(buf);
  for(size_t i = 0;i < len;i++)
    c[i] = (rand() % 10) + '0';
}

void compress()
{
  const int origFd = open(ORIG_FILE_NAME, O_CREAT | O_TRUNC | O_WRONLY, 0666);
  assert(origFd > 0);
  const int zlibFd = open(COMPRESSED_FILE_NAME, O_CREAT | O_TRUNC | O_WRONLY, 0666);
  assert(zlibFd > 0);
  ZLibCompressor compressor;
  for(size_t i = 0;i < TEST_BLOCK_COUNT;i++)
    {
      char buf[TEST_BLOCK_SIZE];
      fillRandomBuf(buf, sizeof(buf));
      const ssize_t written = write(origFd, buf, sizeof(buf));
      assert(written == sizeof(buf));
      char zlibBuf[TEST_BLOCK_SIZE];
      const char* srcBegin = buf;
      const char *srcEnd = srcBegin + TEST_BLOCK_SIZE;
      char * destBegin;
      char * destEnd;
      while(1)
	{
	  destBegin = zlibBuf;
	  destEnd = destBegin + TEST_BLOCK_SIZE;
	  char* origValue = destBegin;
	  compressor.filter(srcBegin, srcEnd, destBegin, destEnd, i == TEST_BLOCK_COUNT - 1);
	  const int has = static_cast<int>(destBegin - origValue);
	  if (has > 0)
	    {
	      const ssize_t written = write(zlibFd, zlibBuf, has);
	      assert(written == has);
	    }
	  if (destEnd != destBegin)
	    break;
	}
      assert(srcBegin == srcEnd);
    }
  close(origFd);
  close(zlibFd);
}

void decompress()
{
  const int origFd = open(COMPRESSED_FILE_NAME, O_RDONLY);
  assert(origFd > 0);
  const int zlibFd = open(DECOMPRESSED_FILE_NAME, O_CREAT | O_TRUNC | O_WRONLY, 0666);
  assert(zlibFd > 0);
  ZLibDecompressor decompressor;
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
      char* destBegin;
      char* destEnd;
      while(1)
	{
	  destBegin = zlibBuf;
	  destEnd = destBegin + TEST_BLOCK_SIZE;
	  char* origValue = destBegin;
	  decompressor.filter(srcBegin, srcEnd, destBegin, destEnd, readCount < (ssize_t)sizeof(buf));//FIXME:eof!!!
	  const int has = static_cast<int>(destBegin - origValue);
	  if (has > 0)
	    {
	      const ssize_t written = write(zlibFd, zlibBuf, has);
	      assert(written == has);
	    }
	  if (destEnd != destBegin)
	    break;
	}
	  assert(srcBegin == srcEnd);
    }
  close(origFd);
  close(zlibFd);
}

int main(int argc, char* argv[])
{
  compress();
  decompress();
  return 0;
}
