/*
   Copyright 2011-2013 ALT Linux
   Copyright 2011-2013 Michael Pozhidaev

   This file is part of the Deepsolver.

   Deepsolver is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   Deepsolver is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
*/

#include"deepsolver.h"
#include"ZLibInterface.h"

#define TEST_BLOCK_SIZE 512
#define TEST_BLOCK_COUNT 16384

#define ORIG_FILE_NAME "orig.data"
#define COMPRESSED_FILE_NAME "compressed.data"
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
  const int zlibFd = open(COMPRESSED_FILE_NAME, O_CREAT | O_TRUNC | O_WRONLY, 0666);
  assert(zlibFd > 0);
  ZLibCompressor compressor;
  for(size_t i = 0;i < TEST_BLOCK_COUNT;i++)
    {
      char buf[TEST_BLOCK_SIZE];
      fillRandomBuf(buf, sizeof(buf));
      const ssize_t written = write(origFd, buf, sizeof(buf));
      assert(written == sizeof(buf));//FIXME:
      const char* srcPos = buf;
      size_t srcProcessed = 0;
      char zlibBuf[TEST_BLOCK_SIZE];
      while(1)
	{
	  assert(srcProcessed <= sizeof(buf));
	  compressor.filter(srcPos, sizeof(buf) - srcProcessed, zlibBuf, sizeof(zlibBuf), i == TEST_BLOCK_COUNT - 1);
	  srcPos = compressor.getSrcPos();
	  srcProcessed += compressor.getSrcProcessed();
	  if (compressor.getDestProcessed() > 0)
	    {
	      const ssize_t written = write(zlibFd, zlibBuf, compressor.getDestProcessed());
	      assert((size_t)written == compressor.getDestProcessed());
	    }
	  if (compressor.getDestProcessed() < sizeof(zlibBuf))
	    break;
	}
      assert(srcProcessed == sizeof(buf));
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
