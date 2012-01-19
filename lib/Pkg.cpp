
#include"depsolver.h"
#include"Pkg.h"

std::ostream& operator <<(std::ostream& s, const NamedPkgRel& r)
{
  s << r.pkgName;
  if (r.ver.empty())
    return s;
  const bool less = r.type & VerLess, equals = r.type & VerEquals, greater = r.type & VerGreater;
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
