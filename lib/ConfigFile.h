
#ifndef DEEPSOLVER_CONFIG_FILE_H
#define DEEPSOLVER_CONFIG_FILE_H

class AbstractConfigFileListener
{
public:
  virtual ~AbstractConfigFileListener() {}

public:
  virtual void onStringValue(const stringVector& path, const std::string& value) = 0;
  virtual void onIntValue(const StringVector& path, int value) = 0;
  virtual void onBooleanValue(const StringVector& path, bool value) = 0;
}; //class AbstractConfigFileListener;

class ConfigFile
{
private:
  class SyntaxItem;
  typedef std::vector<SyntaxItem> SyntaxItemVector;
  typedef std::list<SyntaxItem> SyntaxItemList;

public:
  ConfigFile(AbstractConfigFileListener& listener): m_listener(listener) {}
  virtual ~ConfigFile() {}

public:
  void read(const std::string& fileName);

private:
  void processSyntaxItems(const SyntaxItemVector& items);

private:
  AbstractConfigFileLIstener& m_listener;
}; //class ConfigFile;

#endif //DEEPSOLVER_CONFIG_FILE_H;
