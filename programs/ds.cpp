
#include"depsolver.h"
#include"RepoIndexTextFormatReader.h"
#include"TaskPreprocessor.h"
#include"IndexCore.h"

static clock_t clockStarted;

static void beginClock()
{
  clockStarted = clock();
}

double endClock()
{
  const clock_t val = clock() - clockStarted;
  return (double)val / CLOCKS_PER_SEC;
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
  std::cout << "Package data loaded in " << endClock() << " seconds" << std::endl;
  content.commit();
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
