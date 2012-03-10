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

#ifndef DEEPSOLVER_TASK_EXCEPTION_H
#define DEEPSOLVER_TASK_EXCEPTION_H

//This class must have i18n support in the future;
class TaskException: public DeepsolverException
{
public:
  TaskException(const std::string& msg)
    : m_message(msg) {}

  virtual ~TaskException() {}

public:
  std::string getType() const
  {
    return "task";
  }

  std::string getMessage() const
  {
    return m_message;
  }

private:
  std::string m_message;
}; //class TaskException;

#endif //DEEPSOLVER_TASK_EXCEPTION_H;
