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

#ifndef DEEPSOLVER_ABSTRACT_CONSOLE_MESSAGES_H
#define DEEPSOLVER_ABSTRACT_CONSOLE_MESSAGES_H

class AbstractConsoleMessages
{
public:
  virtual ~AbstractConsoleMessages() {}

public:
  virtual std::ostream& msg() = 0;
  virtual std::ostream& verboseMsg() = 0;
}; //class AbstractConsoleMessages;

#endif //DEEPSOLVER_ABSTRACT_CONSOLE_MESSAGES_H;
