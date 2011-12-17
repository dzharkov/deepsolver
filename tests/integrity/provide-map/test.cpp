
#include"depsolver.h"
#include"TextFiles.h"

struct MapItem
{
  MapItem() {}
  MapItem(const std::string& pkgName, const std::string& provideName)
    : pkg(pkgName), provide(provideName) {}

  std::string pkg, provide;
}; //struct MapItem;

typedef std::list<MapItem> MapItemList;

MapItemList list1;//By rpms.data;
MapItemList list2;//By provides.data;

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

void processPkgFile(const std::string& fileName)
{
  std::cout << "Reading " << fileName << "..." << std::endl;
  std::auto_ptr<AbstractTextFileReader> reader = openIndexFile(fileName);
  std::string line, name;
  while(reader->readLine(line))
    {
      std::string tail;
      if (stringBegins(line, "n=", tail))
	{
	  name = tail;
	  continue;
	}
      if (stringBegins(line, "p:", tail))
	{
	  std::string pkgName = extractPackageName(tail);
	  assert(!pkgName.empty());
	  list1.push_back(MapItem(name, pkgName));
	  provides.insert(pkgName);
	  continue;
	}
    }
}

void processProvidesFile(const std::string& fileName)
{
  std::cout << "Reading " << fileName << "..." << std::endl;
  std::auto_ptr<AbstractTextFileReader> reader = openIndexFile(fileName);
  std::string line, provideName;
  bool wasEmpty = 0;
  while(reader->readLine(line))
    {
      if (line.empty())
	{
	  wasEmpty = 1;
	  continue;
	}
      if (wasEmpty && line.length() > 2 && line[0] == '[' && line[line.length() - 1] == ']')
	{
	  //FIXME:	  provideName = ;
	  wasEmpty = 0;
	  continue;
	}
      assert(!provideName.empty());
      list2.push_back(MapItem(line, provideName));
      wasEmpty = 0;
    }
}

int main(int argc, char* argv[])
{
  assert(argc == 3);
  processPkgFile(argv[1]);
  processProvidesFile(argv[1]);
  return count == 0?0:1;
}
