/*
   Copyright 2011-2013 ALT Linux
   Copyright 2011-2013 Michael Pozhidaev

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

#ifndef DEEPSOLVER_TYPES_H
#define DEEPSOLVER_TYPES_H

/*
 * PackageId type is the identifier of an item from the set built by
 * included strings from set of package names and set of all their
 * provides without any additional information such as version or anything
 * else.  Set of packages to consider consists of three parts: packages
 * from accessible repositories, installed packages in the system and
 * packages provided by user as files in the file system or by URL (may
 * absent in accessible repositories). 
 *
 * This type is basically  used due to performance resons.
 */
typedef size_t PackageId;
#define BAD_PACKAGE_ID (PackageId)-1
typedef std::vector<PackageId> PackageIdVector;
typedef std::list<PackageId> PackageIdList;

/*
 * The VarId type is used to identify one exact package from the set of
 * all known packages. The set of all known packages consists of all
 * packages from the accessible repositories, all installed packages and
 * packages provided by user as files in file system or by URL. Two or
 * more packages can be identify by the same VarId if they have the same
 * name, the same epoch, the same version, the same release and the same
 * build time. In all other cases packages must be considered as
 * different packages and must be identified by different values of
 * VarId. In this meaning values of VarId can be used as variables in SAT
 * formulas.
 *
 * The VarId must not be confused with PackageId type.
 */
typedef size_t VarId;
#define BAD_VAR_ID (VarId)-1
typedef std::vector<VarId> VarIdVector;
typedef std::list<VarId> VarIdList;
typedef std::set<VarId> VarIdSet;
typedef std::map<VarId, VarId> VarIdToVarIdMap;

typedef char VerDirection;

enum {
  VerNone = 0,
  VerLess = 1,
  VerEquals = 2,
  VerGreater = 4
};

class VersionCond
{
public:
  VersionCond()
    : type(VerNone) {}

  VersionCond(const std::string& v)
    : version(v), type(VerEquals) {}

  VersionCond(const std::string& v, VerDirection t)
    : version(v), type(t) {}

  VersionCond(const std::string& v, bool l, bool e, bool g)
    : version(v), type(0)
  {
    assert(!l || !g);
    if (l)
      type |= VerLess;
    if (e)
      type |= VerEquals;
    if (g)
      type |= VerGreater;
  }

public:
  bool isLess() const
  {
    return type & VerLess;
  }

  bool isEqual() const
  {
    return type & VerEquals;
  }

  bool isGreater() const
  {
    return type & VerGreater;
  }

public:
  std::string version;
  VerDirection type;
}; //class VersionCond;

typedef std::list<VersionCond> VersionCondList;
typedef std::vector<VersionCond> VersionCondVector;

//Utils;

template<typename T>
class AutoArrayAdapter
{
public:
  AutoArrayAdapter(T* p)
    : m_p(p) 
  {
    assert(p != NULL);
  }

  ~AutoArrayAdapter()
  {
    delete[] m_p;
  }

public:
  T* get()
  {
    return m_p;
  }

  const T* get() const
  {
    return m_p;
  }

private:
  T* m_p;
}; //class AutoArrayAdapter;

class UserTaskItemToInstall
{
public:
  UserTaskItemToInstall()
    : verDir(VerNone) {}

 UserTaskItemToInstall(const std::string& n)
    : pkgName(n), verDir(VerNone) {}

  UserTaskItemToInstall(const std::string& n, VerDirection d, const std::string& v)
    : pkgName(n), verDir(d), version(v) {}

  virtual ~UserTaskItemToInstall() {}

public:
  std::string toString() const
  {
    if (verDir == VerNone)
      return pkgName;
    std::string s = pkgName;
    s += " ";
    if (verDir & VerLess)
      s += "<";
    if (verDir & VerGreater)
      s += ">";
    if (verDir & VerEquals)
      s += "=";
    return s + " " + version;
  }

public:
  std::string pkgName;
  VerDirection verDir;
  std::string version;
}; //class UserTaskItemToINstall;

typedef std::vector<UserTaskItemToInstall> UserTaskItemToInstallVector;
typedef std::list<UserTaskItemToInstall> UserTaskItemToInstallList;

class UserTask 
{
public:
  UserTask() {}
  ~UserTask() {}

public:
  UserTaskItemToInstallVector itemsToInstall;
  StringSet urlsToInstall;
  StringSet namesToRemove;
}; //class UserTask;


#endif //DEEPSOLVER_TYPES_H;
