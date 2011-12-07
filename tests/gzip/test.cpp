
#include"depsolver.h"
#include"GZipInterface.h"

#define TEST_BLOCK_SIZE 512
#define TEST_BLOCK_COUNT 16384

#define ORIG_FILE_NAME "orig.data"
#define COMPRESSED_FILE_NAME "compressed.data.gz"
#define DECOMPRESSED_FILE_NAME "decompressed.data"

void fillRandomBuf(void* buf, size_t len)
{
  assert(buf);
  unsigned char* c = static_cast<unsigned char*>(buf);
  for(size_t i = 0;i < len;i++)
    c[i] = (rand() % 10) + '0';//Using only digits to make data more suitable for compressing;
}

void compress()
{
  const int origFd = open(ORIG_FILE_NAME, O_CREAT | O_TRUNC | O_WRONLY, 0666);
  assert(origFd > 0);
  GZipOutputFile gzip;
  gzip.open(COMPRESSED_FILE_NAME);
  for(size_t i = 0;i < TEST_BLOCK_COUNT;i++)
    {
      char buf[TEST_BLOCK_SIZE];
      fillRandomBuf(buf, sizeof(buf));
      const ssize_t written = write(origFd, buf, sizeof(buf));
      assert(written == sizeof(buf));//FIXME:
      gzip.write(buf, sizeof(buf));
    }
  close(origFd);
}

void decompress()
{
  /*
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
      const char* srcPos = buf;
      size_t srcProcessed = 0;
      char zlibBuf[TEST_BLOCK_SIZE];
      while(1)
	{
	  assert(readCount != -1 && (size_t)readCount >= srcProcessed);
	  if ((size_t)readCount == srcProcessed)//No data to decompress;
	    break;
	  decompressor.filter(srcPos, (size_t)readCount - srcProcessed, zlibBuf, sizeof(zlibBuf));//FIXME:eof!!!
	  srcPos = decompressor.getSrcPos();
	  srcProcessed += decompressor.getSrcProcessed(); 
	  if (decompressor.getDestProcessed() > 0)
	    {
	      const ssize_t written = write(zlibFd, zlibBuf, decompressor.getDestProcessed());
	      assert((size_t)written == decompressor.getDestProcessed());
	    }
	  if (decompressor.getDestProcessed() != sizeof(zlibBuf))
	    break;
	}
      assert(srcProcessed == (size_t)readCount);
    }
  close(origFd);
  close(zlibFd);
  */
}

int main(int argc, char* argv[])
{
  try {
    std::cout << "Compressing " << (TEST_BLOCK_SIZE * TEST_BLOCK_COUNT) / 1024 << "k of random data" << std::endl;
    compress();
    std::cout << "Decompressing..." << std::endl;
    decompress();
  }
  catch (const DepsolverException& e)
    {
      std::cerr << e.getType() << " error:" << e.getMessage() << std::endl;
      return 1;
    }
  return 0;
}
