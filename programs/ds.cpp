
#include"depsolver.h"
#include"RepoIndexTextFormatReader.h"
#include"TaskPreprocessor.h"
#include"IndexCore.h"

static clock_t clockStarted;

bool       translateDirection(UserTaskItemToInstall& item, const std::string& str)
{
  item.less = 0;
  item.equals = 0;
  item.greater = 0;
  if (str == ">")
    {
      item.greater = 1;
      return 1;
    }
  if (str == "<")
    {
      item.less = 1;
      return 1;
    }
  if (str == ">=")
    {
      item.greater = 1;
      item.equals = 1;
      return 1;
    }
  if (str == "<=")
    {
      item.less = 1;
      item.equals = 1;
      return 1;
    }
  if (str == "=")
    {
      item.equals = 1;
      return 1;
    }
  return 0;
}

bool parseUserTask(const std::string& line, UserTask& task)
{
  std::string::size_type i = 0;
  while(1)
    {
      UserTaskItemToInstall item;
      //Expecting package name but must skip spaces;
      while(i < line.length() && BLANK_CHAR(line[i]))
	i++;
      if (i >= line.length())
	break;
      item.pkgName.erase();
      item.version.erase();
      while(i < line.length() && !BLANK_CHAR(line[i]))
	item.pkgName += line[i++];
      while(i < line.length() && BLANK_CHAR(line[i]))
	i++;
      if (i >= line.length())
	{
	  task.itemsToInstall.push_back(item);
	  break;
	}
      if (line[i] != '<' && line[i] != '>' && line[i] != '=')
	{
	  task.itemsToInstall.push_back(item);
	  continue;
	}
      std::string r;
      r += line[i];
      if (i + 1 < line.length() && !BLANK_CHAR(line[i + 1]))
	{
	  i++;
	  r += line[i];
	}
      if (!translateDirection(item, r))
	return 0;
      i++;
      if (i>= line.length() || !BLANK_CHAR(line[i]))
	return 0;
      while (i < line.length() && BLANK_CHAR(line[i]))
	i++;
      if (i >= line.length())
	return 0;
      while(i < line.length() && !BLANK_CHAR(line[i]))
	item.version += line[i++];
      task.itemsToInstall.push_back(item);
    }
  return 1;
}

void beginClock()
{
  clockStarted = clock();
}

double endClock()
{
  const clock_t val = clock() - clockStarted;
  return (double)val / CLOCKS_PER_SEC;
}

void handleRequest(PackageScope& scope, const std::string& line)
{
  UserTask task;
  if (!parseUserTask(line, task))
    {
      std::cerr << "Incorrect request, be careful!" << std::endl;
	return;
    }
  if (task.itemsToInstall.empty())
    {
      std::cerr << "An empty request!" << std::endl;
      return;
    }
  for(UserTaskItemToInstallVector::size_type i = 0;i < task.itemsToInstall.size();i++)
    std::cout << "# " << task.itemsToInstall[i].makeStr() << std::endl;
  TaskPreprocessor taskPreprocessor(scope);
  VarIdVector toInstall, toRemove;
  beginClock();
  taskPreprocessor.preprocess(task, toInstall, toRemove);
  std::cout << std::endl;
  for(VarIdVector::size_type i = 0;i < toInstall.size();i++)
    std::cout << scope.constructPackageName(toInstall[i]) << std::endl;
  std::cout << std::endl;
  std::cout << "# Answer contains " << toInstall.size() << " entries to install" << std::endl;
  std::cout << "# Calculated in " << endClock() << " sec" << std::endl;
}

void addPackageList(const std::string& dirName, PackageScopeContent& content)
{
  RepoIndexTextFormatReader reader(dirName, RepoIndexParams::CompressionTypeNone);
  PkgFile pkgFile;
  reader.openPackagesFile();
  while(reader.readPackage(pkgFile))
    {
      //      std::cout << pkgFile.name << std::endl;
      content.add(pkgFile);
    }
}

void run()
{
  PackageScopeContent content;
  beginClock();
  addPackageList("i586/base", content);
  addPackageList("noarch/base", content);
  PackageScope scope(content);
  std::cout << "Package data loaded in " << endClock() << " seconds" << std::endl;
  std::string line;
  while(1)
    {
      std::cout << "Your request> ";
      std::getline(std::cin, line);
      if (!std::cin)
	break;
      handleRequest(scope, line);
    }
  std::cout << std::endl;
  std::cout << "Exiting..." << std::endl;
}

int main(int argc, char* argv[])
{
  try {
    run();
  }
  catch (const DepsolverException& e)
    {
      std::cerr << e.getType() << " error:" << e.getMessage() << std::endl;
      return 1;
    }
  return 0;
}
