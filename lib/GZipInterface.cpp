

#include"depsolver.h"
#include<zlib.h>

#define GZIP_STOP(x) throw GZipException(x)

void GZipOutputFile::open(const std::string& fileName)
{
  assert(m_fd == -1);
  m_fd = ::open(fileName.c_str(), O_CREAT | O_TRUNC | O_WRONLY, NEW_FILE_MODE);
  TRY_SYS_CALL(m_fd != -1, "open(" + fileName + ")");
  m_gzfile = gzdopen(m_fd, "w");
  if (m_gzfile == NULL)
    {
      ::close(m_fd);
      m_fd = -1;
      GZIP_STOP("gzdfile(" + fileName + ") cannot create valid gzip file descriptor");
    }
  m_fileName = fileName;
}

void GZipOutputFile::write(const void* buf, size_t bufSize)
{
  assert(m_gzfile != NULL && m_fd != -1);
  gzFile f = static_cast<gzFile>(m_gzfile);

}

void close()
{
  if (m_gzfile == NULL)
    return;
  assert(m_fd != -1);
  gzFile f = static_cast<gzFile>(m_gzfile);
  gzclose(f);
  m_gzfile = NULL;
  m_fd = -1;//gzclose() automatically closes associated file descriptor;
  m_fileName.erase();
}
