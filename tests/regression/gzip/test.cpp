
#include"deepsolver.h"
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
  GZipInputFile gzip;
  gzip.open(COMPRESSED_FILE_NAME);
  const int outputFd = open(DECOMPRESSED_FILE_NAME, O_CREAT | O_TRUNC | O_WRONLY, 0666);
  assert(outputFd > 0);
  while(1)
    {
      char buf[TEST_BLOCK_SIZE];
      const size_t readCount = gzip.read(buf, sizeof(buf));
      if (readCount > 0)
	{
	  const ssize_t written = write(outputFd, buf, readCount);
	  assert((size_t)written == readCount);
	}
      if (readCount < sizeof(buf))
	break;
    }
  close(outputFd);
}

int main(int argc, char* argv[])
{
  try {
    std::cout << "Compressing " << (TEST_BLOCK_SIZE * TEST_BLOCK_COUNT) / 1024 << "k of random data" << std::endl;
    compress();
    std::cout << "Decompressing..." << std::endl;
    decompress();
  }
  catch (const DeepsolverException& e)
    {
      std::cerr << e.getType() << " error:" << e.getMessage() << std::endl;
      return 1;
    }
  return 0;
}
