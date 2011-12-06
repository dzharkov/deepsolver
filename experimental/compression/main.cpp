
#include<assert.h>
#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include"ZLibInterface.h"

#define TEST_BLOCK_SIZE 512
#define TEST_BLOCK_COUNT 1024

/*
 * GZIP header consists of 10  bytes:
 *  - the first magic byte: always 0x1f;
 *  - the second magic byte:: always 0x8b;
 *  - the compression method byte: must be 8 (means 'deflate');
 *  - the flags byte;
 *  - the 4 bytes with time stamp of last modification;
 *  - the extra flags byte;
 *  - the OS type byte.
 *
 * The GZIP suffix consists of 8 bytes:
 *  - the 4 bytes of CRC;
 *  - the 4 bytes of data length.
 */

#define GZIP_HEADER_SIZE 10

//Magic numbers used in gzip header;
#define GZIP_MAGIC1 0x1f
#define GZIP_MAGIC2 0x8b

class GZipFile
{
public:
  enum {//Compression method;
    MethodDeflate = 8
  };

  enum {//Flags;
    FlagText = 1,
    FlagHeaderCRC = 2,
    FlagExtra = 4,
    FlagName  = 8,
    FlagComment = 16
  };

  enum {//Extra flags;
    ExtraFlagBestCompression = 2,
    ExtraFlagBestSpeed  = 4
  };

  enum {//Operating systems;
    OSFat = 0,
    OSAmiga = 1,
    OSVms = 2,
    OSUnix = 3,
    OSVmCms = 4,
    OSAtari = 5,
    OSHpFs = 6,
    OSMacintosh = 7,
    OSZSystem = 8,
    GOSCpM = 9,
    OSTops20 = 10,
    OSNtfs = 11,
    OSQDos = 12,
    OSAcorn = 13,
    OSUnknown = 255
  };

public:
  GZipFile()
    : m_fd(-1) {}
  virtual ~GZipFile() {}

public:
  void open(const std::string& fileName);

private:
  void stop(const std::string& msg) const
  {
    std::cerr << m_fileName << ":" << msg << std::endl;
    exit(EXIT_FAILURE);
  }

  void readHeader();

private:
  int m_fd;//FIXME:replace by 'File' class;
  std::string m_fileName;
  char m_flags, m_extraFlags, m_os;
}; //class GZipFile;

void GZipFile::open(const std::string& fileName)
{
  m_fd = ::open(fileName.c_str(), O_RDONLY);
  assert(m_fd != -1);
  m_fileName = fileName;
  readHeader();
}

void GZipFile::readHeader()
{
  unsigned char header[GZIP_HEADER_SIZE];
  const ssize_t res = ::read(m_fd, header, GZIP_HEADER_SIZE);
  assert(res == GZIP_HEADER_SIZE);
  if (header[0] != GZIP_MAGIC1)
    stop("invalid header first magic byte");
  if (header[1] != GZIP_MAGIC2)
    stop("invalid header second magic byte");
  if (header[2] != MethodDeflate)
    stop("invalid header compression information (expecting \'deflate\')");
  m_flags = header[3];
  //No mtime processing (bytes 4-7): needs additional information about BigEndian/LittleEndian compatibility;
  m_extraFlags = header[8];
  m_os = header[9];
}

void fillRandomBuf(void* buf, size_t len)
{
  assert(buf);
  unsigned char* c = static_cast<unsigned char*>(buf);
  for(size_t i = 0;i < len;i++)
    c[i] = (rand() % 10) + '0';
}

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
  /*
  GZipFile file;
  file.open("data.gz");
  */
  zlibRegression();
  return 0;
}
