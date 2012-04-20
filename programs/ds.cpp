
#include"deepsolver.h"
#include"OperationCore.h"

ConfigCenter conf;

bool loadConfiguration()
{
  conf.loadFromFile("/tmp/ds.ini");
  return 1;
}

int main()
{
  if (!loadConfiguration())
    return 1;
}

