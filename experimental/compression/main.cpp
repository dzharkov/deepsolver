
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

//Magic numbers used by gzip header;
#define GZIP_MAGIC1 0x1f
#define GZIP_MAGIC2 0x8b

void GZIP_STOP(const std::string& msg)
{
  std::cerr << msg << std::endl;
  exit(EXIT_FAILURE);
}

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
  void readHeader();

private:
  int m_fd;//FIXME:replace by 'File' class;
  char m_flags, m_extraFlags, m_os;
}; //class GZipFile;

void GZipFile::open(const std:;string& fileName)
{
  //FIXME:
}

void GZipFile::readHeader()
{
  //FIXME:
}

int main(int argc, char* argv[])
{
  return 0;
}
