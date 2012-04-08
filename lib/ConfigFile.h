
#ifndef DEEPSOLVER_CONFIG_FILE_H
#define DEEPSOLVER_CONFIG_FILE_H

class ConfigFile
{
private:
  class SyntaxItem;
  typedef std::vector<SyntaxItem> SyntaxItemVector;
  typedef std::list<SyntaxItem> SyntaxItemList;

public:
  ConfigFile() {}
  virtual ~ConfigFile() {}

public:
}; //class ConfigFile;

#endif //DEEPSOLVER_CONFIG_FILE_H;
