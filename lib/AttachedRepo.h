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

class AttachedRepo
{
public:
  AttachedRepo() {}
  virtual ~AttachedRepo() {}

public:
  void makeId();

public:
  bool operator ==(const AttachedRepo& r) const
  {
    return id == r.id;
  }

  bool operator !=(const AttachedRepo& r) const
  {
    return id != r.id;
  }

public:
  std::string id;
  std::string url;
  std::string component;
  std::string vendorId;
}; //class AttachedRepo;

typedef std::list<AttachedRepo*> AttachedRepoList;
typedef std::vector<AttachedRepo*> AttachedRepoVector;

#endif //DEEPSOLVER_ATTACHED_REPO_H;
