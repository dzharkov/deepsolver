

#include"ConfigCenter.h"

static std::string collectFiles(const std::string& dirName)
{
  StringVector items;
  DIR* dir = opendir(dirName.c_str());
  if (dir == NULL)
    return std::string();
  struct dirent* ent;
  while ((ent = readdir(dir)) != NULL)
    {
      std::string name(ent->d_name);
      if (name == "." || name == "..")
	continue;
      struct stat s;
      std::string nameToCheck;
      if (dirName.empty() || dirName[dirName.length() - 1] == '/')//FIXME:
	nameToCheck = dirName + name; else
	nameToCheck = dirName + "/" + name;
      if (stat(nameToCheck.c_str(), &s) != 0)
	continue;
      if (!S_ISREG(s.st_mode))//FIXME:
	continue;
      items.push_back(name);
    }
  closedir(dir);
  std::sort(items.begin(), items.end());
  std::string res;
  for(StringVector::size_type i = 0;i < items.size();i++)
    {
      std::string nameToRead;
      if (dirName.empty() || dirName[dirName.length() - 1] == '/')
	nameToRead = dirName + items[i]; else
	nameToRead = dirName + "/" + items[i];//FIXME:
      char c;
      std::string s;
      std::ifstream f(nameToRead.c_str());
      if (!f)
	continue;
      while(f.get(c))
	s += c;
      s += '\n';
      res += s;
    } //for files;
  return res;
}
