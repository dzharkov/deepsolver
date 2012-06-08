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

#ifndef DEEPSOLVER_TASK_EXCEPTION_H
#define DEEPSOLVER_TASK_EXCEPTION_H

enum {
  TaskErrorUnknownPackageName = 1,
  TaskErrorBothInstallRemove = 2,
  TaskErrorNoInstallationCandidat = 3
};

//This class must have i18n support in the future;
class TaskException: public DeepsolverException
{
public:
 TaskException(int code, const std::string& arg)
   : m_code(code), m_arg(arg) {}

  virtual ~TaskException() {}

public:
  int getCode() const
  {
    return m_code;
  }

  const std::string& getArg() const
  {
    return m_arg;
  }

  std::string getType() const
  {
    return "task";
  }

  std::string getMessage() const
  {
    switch(m_code)
      {
      case TaskErrorUnknownPackageName:
	return "unknown package name: " + m_arg;
      case TaskErrorBothInstallRemove:
	return "conflicting actions: \'" + m_arg + "\' was considered both for install and remove";
      case TaskErrorNoInstallationCandidat:
	return "package has no installation candidat: " + m_arg;
      default:
	assert(0);
	return "";//Just to reduce warning 
      }; //switch(m_code);
  }

private:
  const int m_code;
  const std::string m_arg;
}; //class TaskException;

#endif //DEEPSOLVER_TASK_EXCEPTION_H;
