
#ifndef FIXME_FILE_H
#define FIXME_FILE_H

class File
{
public:
  File()
    : m_fd(-1) {}

  virtual ~File()
  {
    close();
  }

  void open(const std::string fileName);
  void openreadOnly(const std::string& fileName);
  void create(const std::string& fileName)
    void close();

  /*
   * Length of the buffer is not limited. In case of large buffer this
   * method produces several subsequent calls of corresponding system
   * function. In addition this method takes care to be sure the operation
   * is performed completely until entire requested data is read or error
   * occured.
   */
    size_t read(void* buf, size_t bufSize) = 0;

  /*
   * Length of the buffer is not limited. In case of large buffer this
   * method produces several subsequent calls of corresponding system
   * function. In addition this method takes care to be sure the operation
   * is performed completely until entire buffer content is written or error
   * occured.
   */
    size_t write(const void* buf, size_t bufSize) = 0;

  int getFd() const
  {
    assert(m_fd >= 0);
    return m_fd;
  }

protected:
  int m_fd;
}; //class File;

#endif //FIXME_FILE_H;
