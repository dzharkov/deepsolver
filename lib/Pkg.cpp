
#include"depsolver.h"
#include"NamedPkgRel.h"

std::ostream& operator <<(std::ostream& s, const NamedPkgRel& r)
{
  s << r.pkgName;
  if (r.ver.empty())
    return s;
  const bool less = r.type & NamedPkgRel::Less, equals = r.type & NamedPkgRel::Equals, greater = r.type & NamedPkgRel::Greater;
  assert(!less || !greater);
  std::string t;
  if (less)
    t += "<";
  if (equals)
    t += "=";
  if (greater)
    t += ">";
  s << " " << t << " " << r.ver;
  return s;
}
