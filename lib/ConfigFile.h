
#ifndef DEEPSOLVER_CONFIG_FILE_H
#define DEEPSOLVER_CONFIG_FILE_H

enum {
  ConfigErrorSectionInvalidType = 0,
  ConfigErrorSectionWaitingOpenBracket = 1,
  ConfigErrorSectionWaitingName = 2,
  ConfigErrorSectionInvalidNameChar = 3,
  ConfigErrorSectionWaitingCloseBracketOrArg = 4,
  ConfigErrorSectionUnexpectedArgEnd = 5,
  ConfigErrorSectionWaitingCloseBracket = 6,
  ConfigErrorValueWaitingName = 7,
  ConfigErrorValueInvalidNameChar = 8,
  ConfigErrorValueWaitingAssignOrNewName = 9,
  ConfigErrorValueWaitingNewName = 10,
  ConfigErrorValueUnexpectedValueEnd = 11
};

class ConfigFileException
{
public:
  ConfigFileException(int code,
		      const std::string& fileName,
		      size_t lineNumber,
		      std::string::size_type pos,
		      const std::string& line)
    : m_code(code),
      m_fileName(fileName),
      m_lineNumber(lineNumber),
      m_pos(pos),
      m_line(line) {}

  virtual ~ConfigFileException() {}

public:
  int getCode() const
  {
    return m_code;
  }

  const std::string& getFileName() const
  {
    return m_fileName;
  }

  size_t getLineNumber() const
  {
    return m_lineNumber;
  }

  std::string::size_type getPos() const
  {
    return m_pos;
  }

  const std::string& getLine() const
  {
    return m_line;
  }

  std::string getType() const
  {
    return "FIXME";
  }

  std::string getMessage() const
  {
    return "FIXME";
  }

private:
  const int m_code;
  const std::string m_fileName;
  const size_t m_lineNumber;
  const std::string::size_type m_pos;
  const std::string m_line;
}; //class ConfigFileException;

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
