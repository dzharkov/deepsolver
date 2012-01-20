
#include"depsolver.h"
#include"RepoIndexTextFormatReader.h"
#include"TaskPreprocessor.h"
#include"IndexCore.h"

void run()
{
  PackageScopeContent content;
  RepoIndexTextFormatReader reader(".", RepoIndexParams::CompressionTypeGzip);
  PkgFile pkgFile;
  reader.openPackagesFile();
  while(reader.readPackage(pkgFile))
    content.add(pkgFile);
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
