
#include"deepsolver.h"
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
  std::cout << "Reading " << fileName << "..." << std::endl;
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

size_t printUnmets()
{
  size_t count = 0;
  std::cout << "Unmets:" << std::endl;
  for(StringSet::const_iterator it = requires.begin();it != requires.end();it++)
    {
      if (provides.find(*it) != provides.end())
	continue;
      std::cout << *it << std::endl;
      count++;
    }
  if (count > 0)
    {
      std::cout << std::endl;
      std::cout << "Total: " << count << std::endl;
    } else
    std::cout << "(none)" << std::endl;
  return count;
}

void printUnusedProvides()
{
  size_t count = 0;
  std::cout << "Unused provides:" << std::endl;
  for(StringSet::const_iterator it = provides.begin();it != provides.end();it++)
    {
      if (requires.find(*it) != requires.end())
	continue;
      std::cout << *it << std::endl;
      count++;
    }
  if (count > 0)
    {
      std::cout << std::endl;
      std::cout << "Total: " << count << std::endl;
    } else
    std::cout << "(none)" << std::endl;
}

int main(int argc, char* argv[])
{
  for(int i = 1;i < argc;i++)
    processIndexFile(argv[i]);
  std::cout << std::endl;
  std::cout << "Requires count: " << requires.size() << std::endl;
  std::cout << "Provides count: " << provides.size() << std::endl;
  std::cout << std::endl;
  printUnusedProvides();
  std::cout << std::endl;
  const size_t count = printUnmets();
  std::cout << std::endl;
  return count == 0?0:1;
}
