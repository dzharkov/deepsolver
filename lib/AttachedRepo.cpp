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

#include"AttachedRepo.h"

void AttachedRepo::init(const std::string initString)
{
  //FIXME:
}

void AttachedRepo::assignId(AttachedRepoId newValue)
{
  assert(newValue != ATTACHED_REPO_BAD_ID);
  assert(m_id == ATTACHED_REPO_BAD_ID);//This operation must be performed only once;
  m_id = newValue;
}

attachedRepoId AttachedRepo::getId() const
{
  assert(m_id != ATTACHED_REPO_BAD_ID);
  return m_id;
}
