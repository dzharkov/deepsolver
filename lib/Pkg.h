
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

std::ostream& operator <<(std::ostream& s, const NamedPkgRel& r);

typedef std::list<NamedPkgRel> NamedPkgRelList;
typedef std::vector<NamedPkgRel> NamedPkgRelVector;

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

#endif //DEEPSOLVER_PKG_H;
