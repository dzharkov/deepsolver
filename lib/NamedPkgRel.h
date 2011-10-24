

#ifndef FIXME_NAMED_PKG_REL_H
#define FIXME_NAMED_PKG_REL_H

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
  enum {
    Less = 1;
    Equals = 2;
    Greater = 4;
  };

  NamedPkgRel()
    : type(0) {}

  NamedPkgRel(const std::string& pName)
    : pkgName(pName), type(0) {}

  NamedPkgRel(const std::string& pName, char t, const std::string& v)
    : pkgName(pName), type(t), ver(v) {}

public:
  std::string pkgName;
  char type;
  std::string ver;
}; //class NamedPkgRel;

#endif //FIXME_NAMED_PKG_REL_H;
