#include"deepsolver.h"
#include"utils/SqliteInterface.h"
#include <sqlite3.h>

void sqliteLog(int level, std::string msg)
{
  std::string resMsg = "SQLite: " + msg;
  logMsg(level, msg.c_str());
}

bool SqliteInterface::init(const std::string& dbFilename) 
{
  int rc;
  sqlite3* dbHandle;
  rc = sqlite3_open(dbFilename.c_str(), &dbHandle);
  if (rc) 
  {
    sqliteLog(LOG_DEBUG, "cannot open db " + dbFilename);
    return false;
  }
  
  db = dbHandle;

  sqliteLog(LOG_DEBUG, "opened db " + dbFilename);
  return true;
}

bool SqliteInterface::execute(const std::string& sqlStatement) 
{
  sqlite3* dbHandle = (sqlite3*)db;
  char* err;
  
  sqliteLog(LOG_DEBUG, "executing query \"" + sqlStatement + "\"");
  if (sqlite3_exec(dbHandle, sqlStatement.c_str(), 0, 0, &err)) 
  {
    sqliteLog(LOG_DEBUG, "execution error (" + std::string(err) + ")");
    return false;
  }  	
  return false;
}
