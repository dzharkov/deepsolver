
#ifndef DEPSOLVER_PKG_H
#define DEPSOLVER_PKG_H

#include"NamedPkgRel.h"
#include"ChangeLog.h"

typedef unsigned short Epoch;

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
    : source(0)  {}

  virtual ~PkgFileBase() {}

public:
  std::string fileName;
  bool source;
}; //class PkgFileBase;

class PkgRelations
{
public:
  NamedPkgRelVector requires, provides, conflicts, obsoletes;
}; //class PkgRelations;

class Pkg: public PkgBase, public PkgRelations
{
public:
  ChangeLog changeLog;
};

class PkgFile: public PkgFileBase, public PkgRelations 
{
public:
  ChangeLog changeLog;
};

#endif //DEPSOLVER_PKG_H;
