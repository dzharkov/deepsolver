
#include<assert.h>
#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>

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

int main(int argc, char* argv[])
{
  GZipFile file;
  file.open("data.gz");
  return 0;
}
