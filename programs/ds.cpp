
#include"deepsolver.h"
#include"RepoIndexTextFormatReader.h"
#include"TaskPreprocessor.h"
#include"IndexCore.h"
#include"TaskException.h"
#include"PackageScopeContentBuilder.h"
#include"PackageScopeContentLoader.h"

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

void handleRequest(PackageScope& scope, const std::string& line)
{
  UserTask task;
  if (!parseUserTask(line, task))
    {
      std::cerr << "Incorrect request, be careful!" << std::endl;
	return;
    }
  if (task.itemsToInstall.empty())
    return;
  ProvidePriorityList priorityList;
  priorityList.load("pkgpriorities");
  TaskPreprocessor taskPreprocessor(scope, priorityList);
  VarIdVector toInstall, toRemove;
  try {
    taskPreprocessor.preprocess(task, toInstall, toRemove);
  }
  catch (const TaskException& e)
    {
      std::cout << e.getMessage() << std::endl;
      return;
    }
  std::ofstream tmpRes("/tmp/deepsolver.result.txt");
  std::cout << std::endl;
  for(VarIdVector::size_type i = 0;i < toInstall.size();i++)
    {
      std::cout << scope.constructPackageName(toInstall[i]) << std::endl;
      tmpRes << scope.constructPackageName(toInstall[i]) << std::endl;
    }
  std::cout << std::endl;
  VarIdSet closure, conflicts;
  try {
    for(VarIdVector::size_type i = 0;i < toInstall.size();i++)
      taskPreprocessor.buildDepClosure(toInstall[i], closure, conflicts);
  }
  catch (const TaskException& e)
    {
      std::cout << e.getMessage() << std::endl;
      return;
    }
  StringVector answer;
  if (!closure.empty())
    {
      std::cout << "# Dependent packages:" << std::endl;
      std::cout << std::endl;
      for(VarIdSet::const_iterator it = closure.begin();it != closure.end();it++)
	answer.push_back(scope.constructPackageName(*it));
      std::sort(answer.begin(), answer.end());
      for(StringVector::size_type i = 0;i < answer.size();i++)
	{
	  std::cout << answer[i] << std::endl;
	  tmpRes << answer[i] << std::endl;
	}
      std::cout << std::endl;
    } else 
    std::cout << "# No dependent packages" << std::endl << std::endl;
  for(VarIdVector::size_type i = 0;i < toInstall.size();i++)
    closure.insert(toInstall[i]);//We must explicitly do it since closure contains only dependent packages;
  //Conflicts;
  if (!conflicts.empty())
    {
      std::cout << "# Conflicted packages:" << std::endl;
      std::cout << std::endl;
      answer.clear();
      for(VarIdSet::const_iterator it = conflicts.begin();it != conflicts.end();it++)
	answer.push_back(scope.constructPackageName(*it));
      std::sort(answer.begin(), answer.end());
      for(StringVector::size_type i = 0;i < answer.size();i++)
	std::cout << answer[i] << std::endl;
      std::cout << std::endl;
      //Required and conflicted entries intersection;
      answer.clear();
      for(VarIdSet::const_iterator it1 = closure.begin();it1 != closure.end();it1++)
	{
	  VarIdSet::const_iterator it2 = conflicts.find(*it1);
	  if (it2 != conflicts.end())
	    answer.push_back(scope.constructPackageName(*it1));
	}
      if (!answer.empty())
	{
	  std::cout << "# The following packages are presented both in requires and conflicts sets:" << std::endl;
	  std::cout << std::endl;
	  for(StringVector::size_type i = 0;i < answer.size();i++)
	    std::cout << answer[i] << std::endl;
	  std::cout << std::endl;
	} else 
	std::cout << "# No packages included both in requires and conflicts sets" << std::endl << std::endl;
    } else
    std::cout << "# No conflicted packages" << std::endl << std::endl;
  //Answer footer;
  std::cout << "# Answer contains " << closure.size() << " entries to install and " << conflicts.size() << " conflicted entries" << std::endl;
}

void addPackageList(const std::string& dirName, PackageScopeContentBuilder& content)
{
  RepoIndexTextFormatReader reader(dirName, RepoIndexParams::CompressionTypeNone);
  PkgFile pkgFile;
  reader.openPackagesFile();
  while(reader.readPackage(pkgFile))
      content.addPkg(pkgFile);
}

void addProvides(const std::string& dirName, PackageScopeContentBuilder& content)
{
  RepoIndexTextFormatReader reader(dirName, RepoIndexParams::CompressionTypeNone);
  std::string provideName;
  StringVector providers;
  reader.openProvidesFile();
  while(reader.readProvides(provideName, providers))
    {
      assert(!provideName.empty());
      for(StringVector::size_type i = 0;i < providers.size();i++ )
	content.addProvideMapItem(provideName, providers[i]);
    }
}

void buildBinaryData()
{
  std::cout << "Creating binary index... ";
  PackageScopeContentBuilder content;
  addPackageList("i586/base", content);
  addPackageList("noarch/base", content);
  addProvides("i586/base", content);
  addProvides("noarch/base", content);
  content.commit();
  content.saveToFile(PACKAGE_LIST_FILE_NAME);
  std::cout << "done." << std::endl;
}

void run(int argc, char* argv[])
{
  if (argc == 2 && std::string(argv[1]) == "update")
    {
      buildBinaryData();
      return;
    }
  PackageScopeContentLoader content;
  content.loadFromFile(PACKAGE_LIST_FILE_NAME);
  PackageScope scope(content);
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
  initLogging("/tmp/deepsolver.log", LOG_DEBUG);//FIXME:
  try {
    run(argc, argv);
  }
  catch (const DeepsolverException& e)
    {
      std::cerr << e.getType() << " error:" << e.getMessage() << std::endl;
      return 1;
    }
  return 0;
}
