
#ifndef DEPSOLVER_SYSTEM_EXCEPTION_H
#define DEPSOLVER_SYSTEM_EXCEPTION_H

/**\brief The exception for system call errors
 *
 * This class is used for indication of errors caused by various system
 * calls problems. It automatically analyzes value of system errno
 * variable and can construct informative error description with text
 * provided by operating system about error occurred. The error message
 * consists of two parts: the short string provided by developer with any
 * information his want and the string given by the operating system. The
 * developer can save in the string, for example, the name of system call
 * with its arguments which execution failed.
 *
 * \sa Exception
 */
class SystemException: public DepsolverException
{
public:
  /**\brief The default constructor
   *
   * developer string, so the getMessage() method will return only the
   * single line description provided by operating system.
   */
  SystemException()
    : m_code(errno) {}

  /**\brief The constructor with automatic errno analyzing with developer commant
   *
   * This constructor allows developer to give short comment of the error
   * and automatically add string from operating system got with errno
   * variable.
   *
   * \param [in] comment The developer error comment*
   */
  SystemException(const std::string& comment)
    : m_code(errno), m_comment(comment) {}

  /**\brief The constructor with error code specification
   *
   * Using this constructor the developer can provide error code by himself
   * without any additional comment. The operating system will be requested
   * for error description using provided error code. The developer must
   * give the value usually got through errno variable.
   *
   * \param [in] code The error code
   */
  SystemException(int code)
    : m_code(code) {}

  /**\brief The constructor with error code and comment specification
   *
   * With this constructor developer can provide the error code to request
   * description about from operation system and any short comment he
   * want. The value of error code must be the value usually taken through
   * errno variable.
   * \param [in] code The error code
   * \param [in] comment The developer error additional information
   */
  SystemException(int code, const std::string& comment)
    : m_code(code), m_comment(comment) {}

  /**\brief The destructor*/
  virtual ~SystemException() {}

public:
  /**\brief Returns the type of an error
   *
   * This method always returns the "system" string. This string is used in
   * error message construction and allows to distinguish the system errors
   * from other types of exceptions.
   *
   * \return The single word exception type ("system")
   */
  std::string getType() const
  {
    return "system";
  }

  /**\brief Returns the error code
   *
   * This method returns the errno value got at exception creations.
   *
   * \return The erno value of corresponding error
   */
  int getCode() const
  {
    return m_code;
  }

  /**\brief The error description from the operating system
   *
   * This method returns the error description from the operating system
   * got by the errno value with strerror[] array.
   *
   * \return The error description from the operating system
   */
  std::string getDescr() const
  {
    return strerror(m_code);
  }

  /**\brief Returns the developer error comment
   *
   * This method returns the error comment provided by developer.
   *
   * \return The developer error description
   */
  std::string getComment() const
  {
    return m_comment;
  }

  /**\brief The full error description
   *
   * This method constructs complete error description using both the
   * developer comment and the string description from the operating
   * system. Usually this value is well-suited to be printed to user.
   *
   * \return The complete error description
   */
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

#endif //DEPSOLVER_SYSTEM_EXCEPTION_H
