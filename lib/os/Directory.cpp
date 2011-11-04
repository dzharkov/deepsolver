
#include"basic-header.h"//FIXME:

bool Directory::isExist(const std::string& path)
{
  struct stat s;
  TRY_SYS_CALL(stat(path.c_str(), &s) == 0, "stat(" + path + ")");
  return S_ISDIR(s.st_mode);
}

void Directory::ensureExists(const std::string& path)
{
  assert(!path.empty());
  std::string s;
  for(std::string::size_type i = 0;i < path.length();i++)
    {
      if (path[i] == '/')
	{
	  if (s.empty() || s[s.length() - 1] == '/')
	    continue;
	  if (!isExist(s))
	    TRY_SYS_CALL(mkdir(s.c_str(), 0777) == 0, "mkdir(" + s + ")");
	  s += '/';
	}
      s += path[i];
    }
	  if (!isExist(s))
	    TRY_SYS_CALL(mkdir(s.c_str(), 0777) == 0, "mkdir(" + s + ")");
}
