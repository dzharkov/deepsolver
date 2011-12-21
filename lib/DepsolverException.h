
#ifndef DEPSOLVER_DEPSOLVER_EXCEPTION_H
#define DEPSOLVER_DEPSOLVER_EXCEPTION_H

/**\brief The main exception class of Depsolver project
 *
 * The every exception class used for error indication in Depsolver
 * project must be the child (not strongly directly) of this
 * DepsolverException class. It is created to simplify error handling and
 * make it unified. The main information this class must provide is the
 * error type and single line error description.
 *
 * \sa SystemException RpmException IndexCoreException
 */
class DepsolverException
{
public:
  /**\brief The default constructor*/
  DepsolverException() {}

  /**\brief the destructor*/
  virtual ~DepsolverException() {}

public:
  /**\brief Returns the string with error type
   *
   * This method returns the short string with one or two words describing
   * the error type. For example, this method can return values like
   * "system", "rpm" etc. The value returned by this method usually is used
   * for error message construction.
   *
   * \return The short string with error type description
   */
  virtual std::string getType() const = 0;

  /**\brief Returns the single line error description
   *
   * This method returns the single line string value with error
   * description. Usually it is the value printed to user in error
   * message. The value may not include error type since it can be obtained
   * with getType() method.
   *
   * \return The single line error description
   */
  virtual std::string getMessage() const = 0;
}; //class DepsolverException;

#endif //DEPSOLVER_DEPSOLVER_EXCEPTION_H;
