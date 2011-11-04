
#ifndef FIXME_SYSTEM_EXCEPTION_H
#define FIXME_SYSTEM_EXCEPTION_H

class SystemException
{
public:
  SystemException()
    : m_code(errno) {}

  SystemException(const std::string& comment)
    : m_code(errno), m_comment(comment) {}

  SystemException(int code)
    : m_code(code) {}

  SystemException(int code, const std::string& comment)
    : m_code(code), m_comment(comment) {}

  virtual ~SystemException() {}

  int getCode() const
  {
    return m_code;
  }

  std::string getDescr() const
  {
    return strerror(m_code);
  }

  std::string getComment() const
  {
    return m_comment;
  }

  std::string getMessage() const
  {
    return getComment()+":"+getDescr();
  }

private:
  int m_code;
  std::string m_comment;
}; //class SystemException;

#define TRY_SYS_CALL(expr, msg) if (!(expr)) throw SystemException(msg)
#define SYS_STOP(msg) throw SystemException(msg)

#endif //FIXME_SYSTEM_EXCEPTION_H
