
#include<iostream>
#include<assert.h>
#include"Package.h"

std::ostream& operator <<(std::ostream& s, const Package& p)
{
  s << p.name << "-" << p.version << "-" << p.release;
  return s;
}

std::ostream& operator <<(std::ostream& s, const PkgRel& p)
{
  s << p.name;
  if (p.versionRel)
    {
      assert(!p.version.empty());
      s << " ";
      switch(p.versionRel)
	{
	case PkgRel::Less:
	  s << "<";
	  break;
	case PkgRel::LessOrEqual:
	  s << "<=";
	  break;
	case PkgRel::Equal:
	  s << "=";
	  break;
	case PkgRel::GreaterOrEqual:
	  s << ">=";
	  break;
	case PkgRel::Greater:
	  s << ">";
	  break;
	default:
	  assert(0);
	} //switch(p.versionRel); 
      s << " " << p.version;
    } else 
    {
      assert(p.version.empty());
    }
  s << " (" << p.flags << ")";
  return s;
}
