
#include"depsolver.h"
#include"TextFiles.h"

StringSet requires, provides;

std::auto_ptr<AbstractTextFileReader> openIndexFile(const std::string& fileName)
{
  if (checkExtension(fileName, ".gz"))
    return createTextFileReader(TextFileGZip, fileName);
  return createTextFileReader(TextFileStd, fileName);
}


//Package is string beginning until first space without previously used backslash;
std::string extractPackageName(const std::string& line)
{
  std::string s;
  for(std::string::size_type i = 0;i < line.length();i++)
    {
      if (line[i] == '\\')
	{
	  if (i + 1 < line.length())
	      s += line[++i]; else
	    s += "\\";
	  continue;
	}
      if (line[i] == ' ')
	return s;
      s += line[i];
    }
  return s;
}

void processIndexFile(const std::string& fileName)
{
  std::auto_ptr<AbstractTextFileReader> reader = openIndexFile(fileName);
  std::string line;
  while(reader->readLine(line))
    {
      std::string tail;
      if (stringBegins(line, "p:", tail))
	{
	  std::string pkgName = extractPackageName(tail);
	  assert(!pkgName.empty());
	  provides.insert(pkgName);
	  continue;
	}
      if (stringBegins(line, "r:", tail))
	{
	  std::string pkgName = extractPackageName(tail);
	  assert(!pkgName.empty());
	  requires.insert(pkgName);
	  continue;
	}
    }
}

int main(int argc, char* argv[])
{
  for(int i = 1;i < argc;i++)
    processIndexFile(argv[i]);
  std::cout << "Requires count: " << requires.size() << std::endl;
  std::cout << "Provides count: " << provides.size() << std::endl;
  return 0;
}
