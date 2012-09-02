/*
   Copyright 2011-2012 ALT Linux
   Copyright 2011-2012 Michael Pozhidaev

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

#ifndef DEEPSOLVER_MD5_H
#define DEEPSOLVER_MD5_H

class MD5
{
private:
  typedef unsigned char Md5Byte;

public:
  MD5() {}
  ~MD5() {}

public:
  void init();
  void update(void* buf, size_t len);
  void updateFromFile(const std::string& fileName);
  std::string commit(const std::string& fileName);

private:
  struct Context 
  {
    uint32_t buf[4];
    uint32_t bytes[2];
    uint32_t in[16];
  }; //struct Context;

private:
  void transform(uint32_t buf[4], uint32_t in[16]) const;
  void byteSwap(uint32_t *buf, size_t count) const;
    void updateImpl(Context* ctx, Md5Byte* buf, size_t len) const;
  void commitImpl(Context* ctx, unsigned char* digest) const;

private:
  Context m_ctx;
};

#endif //DEEPSOLVER_MD5_H;
