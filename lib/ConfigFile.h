
#ifndef DEEPSOLVER_CONFIG_FILE_H
#define DEEPSOLVER_CONFIG_FILE_H

enum {
  ConfigErrorInvalidSectionType = 0
};

class AbstractConfigFileHandler
{
public:
  virtual ~AbstractConfigFileHandler() {}

public:
  virtual void onConfigFileValue(const StringVector& path, 
				 const std::string& sectArg,
				 const std::string& value,
				 bool adding) = 0;
}; //class AbstractConfigHandler;

class ConfigFile
{
private:
  enum {
    ModeAssign = 0,
    ModeAdding = 1
  };

public:
  ConfigFile(AbstractConfigFileHandler& handler) 
    : m_handler(handler),
      m_linesProcessed(0),
      m_sectLevel(0),
      m_sectArgPos(0),
      m_assignMode(ModeAssign) {}

  virtual ~ConfigFile() {}

public:
  void processLine(const std::string& line);

public:
  void processSection(const std::string& line);
  void processValue(const std::string& line);
  void stopSection(int state, std::string::size_type pos, char ch, const std::string& line);
  void stopParam(int state, std::string::size_type pos, char ch, const std::string& line);

private:
  AbstractConfigFileHandler& m_handler;
  size_t m_linesProcessed;
  StringVector m_path;
  StringVector::size_type m_sectLevel;
  std::string m_sectArg, m_paramValue;
  std::string::size_type m_sectArgPos;
  int m_assignMode;
}; //class ConfigFile;

#endif //DEEPSOLVER_CONFIG_FILE_H
