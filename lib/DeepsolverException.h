/*
   Copyright 2011-2012 ALT Linux
   Copyright 2011-2012 Dmitry V. Levin
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

#ifndef DEEPSOLVER_DEEPSOLVER_EXCEPTION_H
#define DEEPSOLVER_DEEPSOLVER_EXCEPTION_H

/**\brief The main exception class of Deepsolver project
 *
 * The every exception class used for error indication in Deepsolver
 * project must be the child (not strongly directly) of this
 * DeepsolverException class. It is created to simplify error handling and
 * make it unified. The main information this class must provide is the
 * error type and single line error description.
 *
 * \sa SystemException RpmException IndexCoreException
 */
class DeepsolverException
{
public:
  /**\brief The default constructor*/
  DeepsolverException() {}

  /**\brief the destructor*/
  virtual ~DeepsolverException() {}

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
}; //class DeepsolverException;

#endif //DEEPSOLVER_DEEPSOLVER_EXCEPTION_H;
