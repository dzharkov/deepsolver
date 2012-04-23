
#include"deepsolver.h"
#include"OperationCore.h"
#include"Messages.h"

ConfigCenter conf;

bool loadConfiguration()
{
  try{
  conf.loadFromFile("/tmp/ds.ini");
  }
  catch (const ConfigFileException& e)
    {
      Messages(std::cerr).onConfigSyntaxError(e);
      return 0;
    }
  catch (const ConfigException& e)
    {
      Messages(std::cerr).onConfigError(e);
      return 0;
    }
  catch(const SystemException& e)
    {
      Messages(std::cerr).onSystemError(e);
      return 0;
    }
  return 1;
}

int main()
{
  if (!loadConfiguration())
    return 1;
}

