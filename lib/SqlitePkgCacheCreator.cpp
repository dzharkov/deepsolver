
#include "deepsolver.h"
#include "SqlitePkgCacheCreator.h"
#include "utils/SqliteInterface.h"

bool SqlitePkgCacheCreator::createNewCacheDatabase() 
{
  SqliteInterface sq;

  std::string dbFileName = "test.db";
  if (!sq.init(dbFileName)) 
  {
      return false;
  }
  
  if (!sq.execute(
      "DROP TABLE  IF EXISTS `packages`; \
       CREATE VIRTUAL TABLE `packages` using fts4(`name`, `pckgr`, `summ`, `descr`)"
      )
    ) 
  {
      return false;
  }
  
  return true;
}
