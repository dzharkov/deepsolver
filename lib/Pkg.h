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

#ifndef DEEPSOLVER_PKG_H
#define DEEPSOLVER_PKG_H

typedef unsigned short Epoch;

/**\brief The relation between two packages with package specifications by name
 *
 * This class contains information about one relaytion between two
 * packages. Relation data includes version specification and relation
 * type with values from the list "less", "less or equals", "equals",
 * "greater or equals", "greater". One package from the relation defined
 * explicitly by its name in this class, the second one defined
 * implicitly by the owner of this class instance. Package specification
 * by its name is not the single way, since it can take a lot of memory
 * to store package name strings. In some cases the better way to use
 * package identifiers instead of real name strings. The usual purpose of
 * this class is to save data about package provides, requires, conflicts
 * or obsoletes entries.
 */
class NamedPkgRel
{
public:
  NamedPkgRel()
    : type(0) {}

  NamedPkgRel(const std::string& pName)
    : pkgName(pName), type(0) {}

  NamedPkgRel(const std::string& pName, VerDirection t, const std::string& v)
    : pkgName(pName), type(t), ver(v) {}

public:
  std::string pkgName;
  VerDirection type;
  std::string ver;
}; //class NamedPkgRel;

typedef std::list<NamedPkgRel> NamedPkgRelList;
typedef std::vector<NamedPkgRel> NamedPkgRelVector;

class IdPkgRel
{
public:
  IdPkgRel()
    : pkgId(BAD_PACKAGE_ID), verDir(VerNone) {}

  IdPkgRel(PackageId id)
    : pkgId(id), verDir(VerNone) {}

  IdPkgRel(PackageId id, const std::string& v)
    : pkgId(id), verDir(VerEquals), ver(v) {}

  IdPkgRel(PackageId id, VerDirection dir, const std::string& v)
    : pkgId(id), verDir(dir), ver(v) {}

  IdPkgRel(PackageId id, const VersionCond& cond)
    : pkgId(id), verDir(cond.type), ver(cond.version) {}

public:
  bool operator ==(const IdPkgRel& rel) const
  {
    return pkgId == rel.pkgId && verDir == rel.verDir && ver == rel.ver;
  }

  bool operator !=(const IdPkgRel& rel) const
  {
    return pkgId != rel.pkgId || verDir != rel.verDir || ver == rel.ver;
  }


public:
  bool hasVer() const
  {
    if (verDir == VerNone)
      {
	assert(ver.empty());
	return 0;
      }
    assert(!ver.empty());
    return 1;
  }

  std::string verString() const
  {
    if (!hasVer())
      return "";
    std::string s;
    if (verDir & VerLess)
      s += "<";
    if (verDir & VerGreater)
      s += ">";
    if (verDir & VerEquals)
      s += "=";
    s += " " + ver;
    return s;
  }

  VersionCond extractVersionCond() const
  {
    return VersionCond(ver, verDir);
  }

public:
  PackageId pkgId;
  VerDirection verDir;
  std::string ver;
}; //class IdPkgRel;

typedef std::vector<IdPkgRel> IdPkgRelVector;
typedef std::list<IdPkgRel> IdPkgRelList;

class ChangeLogEntry
{
public:
  ChangeLogEntry()
    : time(0) {}

  ChangeLogEntry(time_t tm, const std::string& nm, const std::string& txt)
    : time(tm), name(nm), text(txt) {}

public:
  time_t time;
  std::string name;
  std::string text;
}; //class ChangeLogEntry;

typedef std::vector<ChangeLogEntry> ChangeLogEntryVector;
typedef std::list<ChangeLogEntry> ChangeLogEntryList;
typedef ChangeLogEntryVector ChangeLog;

class PkgBase
{
public:
  PkgBase()
    : epoch(0), buildTime(0)  {}
  virtual ~PkgBase() {}

public:
  Epoch epoch;
  std::string name;
  std::string version;
  std::string release;
  std::string arch;
  std::string packager;
  std::string group;
  std::string url;
  std::string license;
  std::string srcRpm;
  std::string summary;
  std::string description;
  time_t buildTime;
}; //class PkgBase;

class PkgFileBase: public PkgBase
{
public:
  PkgFileBase()
    : isSource(0)  {}

  virtual ~PkgFileBase() {}

public:
  std::string fileName;
  bool isSource;
}; //class PkgFileBase;

class PkgRelations
{
public:
  NamedPkgRelVector requires, provides, conflicts, obsoletes;
  StringVector fileList;
}; //class PkgRelations;

class Pkg: public PkgBase, public PkgRelations
{
public:
  ChangeLog changeLog;
};

typedef std::vector<Pkg> PkgVector;
typedef std::list<Pkg> PkgList;

class PkgFile: public PkgFileBase, public PkgRelations 
{
public:
  ChangeLog changeLog;
};

typedef std::vector<PkgFile> PkgFileVector;
typedef std::list<PkgFile> PkgFileList;

std::ostream& operator <<(std::ostream& s, const PkgBase& );
std::ostream& operator <<(std::ostream& s, const NamedPkgRel& r);

enum {
  PkgFlagAvailableByRepo = 1,
  PkgFlagAvailableByUrl = 2,
PkgFlagInstalled = 4
};

#endif //DEEPSOLVER_PKG_H;
