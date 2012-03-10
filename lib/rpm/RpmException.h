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

#ifndef DEEPSOLVER_RPM_EXCEPTION_H
#define DEEPSOLVER_RPM_EXCEPTION_H

class RpmException: public DeepsolverException
{
public:
  RpmException() {}
  RpmException(const std::string& message)
    : m_message(message
) {}

      virtual ~RpmException() {}

public:
  std::string getType() const
  {
    return "rpm";
  }

  std::string getMessage() const
  {
    return m_message;
  }

private:
  std::string m_message;
}; //class RpmException;

#define RPM_STOP(x) throw RpmException(x)

#endif //DEEPSOLVER_RPM_EXCEPTION_H;
