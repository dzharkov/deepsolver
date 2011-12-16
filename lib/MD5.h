
#ifndef DEPSOLVER_MD5_H
#define DEPSOLVER_MD5_H

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

#endif //DEPSOLVER_MD5_H;
