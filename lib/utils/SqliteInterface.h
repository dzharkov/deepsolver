#ifndef DEEPSOLVER_SQLITE_INTERFACE_H
#define DEEPSOLVER_SQLITE_INTERFACE_H

class SqliteInterface
{
public:
	SqliteInterface() {}
	bool init(const std::string& dbFilename);
	bool execute(const std::string& sqlStatement);
private: 
	void* db;

}; //class SqliteInterface;

#endif //DEEPSOLVER_SQLITE_INTERFACE_H;
