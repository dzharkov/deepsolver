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

#ifndef DEEPSOLVER_ATTACHED_REPO_H
#define DEEPSOLVER_ATTACHED_REPO_H

typedef size_t AttachedRepoId;
#define ATTACHED_REPO_BAD_ID (AttachedRepoId)-1

class AttachedRepo
{
public:
 AttachedRepo():
    m_id(ATTACHED_REPO_BAD_ID), m_flags(0) {}

public:
  void init(const std::string initString);
  void assignId(AttachedRepoId newValue);
  attachedRepoId getId() const;

private:
  AttachedRepoId m_id;
  int m_flags;
  std::string m_url;
  std::string m_signId;
}; //class AttachedRepo;

typedef std::list<AttachedRepo*> AttachedRepoList;
typedef std::vector<AttachedRepo*> AttachedRepoVector;

#endif //DEEPSOLVER_ATTACHED_REPO_H;
