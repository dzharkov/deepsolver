

#include"deepsolver.h"
#include"ConfigFile.h"

//Section header parsing states;
#define SECT_INITIAL 0
#define SECT_BEFORE_NAME 1
#define SECT_NAME 2
#define SECT_AFTER_NAME 3
#define SECT_ARG 4
#define SECT_AFTER_ARG 5

//Value line parsing states;
#define PARAM_INITIAL 0
#define PARAM_NAME 1
#define PARAM_AFTER_NAME 2
#define PARAM_NEXT_NAME 3
#define PARAM_VALUE 4

static void stop(int code,
		 size_t lineNumber,
		 std::string::size_type pos,
		 const std::string& line)
{
  //FIXME:
  std::cout << "Error " << code << " at line " << lineNumber << std::endl;
  std::cout << line << std::endl;
  for(size_t i = 0;i < pos;i++)
    std::cout << " ";
  std::cout << "^" << std::endl;
}

inline bool validIdentChar(char c)
{
  if (c >= 'a' && c <= 'z')
    return 1;
  if (c >= 'A' && c <= 'Z')
    return 1;
  if (c >= '0' && c <= '9')
    return 1;
  if (c == '_' || c == '-' || c == '+')
    return 1;
  return 0;
}

void ConfigFile::processLine(const std::string& line)
{
  std::string::size_type firstChar = 0;
  while(firstChar < line.length() && BLANK_CHAR(line[firstChar]))
    firstChar++;
  if (firstChar >= line.length())
    {
      m_linesProcessed++;
      return;
    }
  if (line[firstChar] == '[')
    {
      processSection(line); 
      assert(!m_path.empty());
      if (!m_sectArg.empty() && m_path.size() > 1)
	{
	  stop(ConfigErrorInvalidSectionType, m_linesProcessed + 1, m_sectArgPos, line);
	  assert(0);
	}
      m_sectLevel = m_path.size();
    } else
    {
      processValue(line);
      assert(m_path.size() >= m_sectLevel);
      if (m_path.size() > m_sectLevel)//It is not an empty line or there is anything than comments;
	{
	  assert(!m_path.empty());
	  assert(m_assignMode == ModeAssign || m_assignMode == ModeAdding);
	  m_handler.onConfigFileValue(m_path, m_sectArg, m_paramValue, m_assignMode == ModeAdding);
	}
      m_path.resize(m_sectLevel);
    }
  m_linesProcessed++;
}

void ConfigFile::processSection(const std::string& line)
{
  m_path.clear();
  m_sectArg.erase();
  int state = SECT_INITIAL;
  std::string value;
  for(std::string::size_type i = 0;i < line.length();i++)
    {
      const char c = line[i];
      //SECT_INITIAL;
      if (state == SECT_INITIAL)
	{
	  if (BLANK_CHAR(c))
	    continue;
	  if (c == '[')
	    {
	      state = SECT_BEFORE_NAME;
	      continue;
	    }
	  stopSection(state, i, c, line);
	  assert(0);
	} //SECT_INITIAL;
      //SECT_BEFORE_NAME;
      if (state == SECT_BEFORE_NAME)
	{
	  if (BLANK_CHAR(c))
	    continue;
	  if (validIdentChar(c))
	    {
	      value.erase();
	      value += c;
	      state = SECT_NAME;
	      continue;
	    }
	  stopSection(state, i , c, line);
	  assert(0);
	} //SECT_BEFORE_NAME;
      //SECT_NAME;
      if (state == SECT_NAME)
	{
	  if (validIdentChar(c))
	    {
	      value += c;
	      continue;
	    }
	  if (BLANK_CHAR(c))
	    {
	      m_path.push_back(value);
	      value.erase();
	      state = SECT_AFTER_NAME;
	      continue;
	    }
	  if (c == '.')
	    {
	      m_path.push_back(value);
	      value.erase();
	      state = SECT_BEFORE_NAME;
	      continue;
	    }
	  if (c == ']')
	    {
	      m_path.push_back(value);
	      return;
	    }
	  stopSection(state, i, c, line);
	  assert(0);
	} //SECT_NAME;
      //SECT_AFTER_NAME;
      if (state == SECT_AFTER_NAME)
	{
	  if (BLANK_CHAR(c))
	    continue;
	  if (c == ']')
	    return;
	  if (c == '.')
	    {
	      state = SECT_BEFORE_NAME;
	      continue;
	    }
	  if (c == '\"')
	    {
	      state = SECT_ARG;
	      m_sectArg.erase();
	      m_sectArgPos = i;
	      continue;
	    }
	  stopSection(state, i, c, line);
	  assert(0);
	} //SECT_AFTER_NAME; 
      //SECT_ARG;
      if (state == SECT_ARG)
	{
	  if (c == '"')
	    {
	      state = SECT_AFTER_ARG;
	      continue;
	    }
	  if (c == '\\')
	    {
	      if (i + 1 >= line.length())
		break;
	      m_sectArg += line[i + 1];
	      i++;
	      continue;
	    }
	  m_sectArg += c;
	  continue;
	} //SECT_ARG;
      //SECT_AFTER_ARG;
      if (state == SECT_AFTER_ARG)
	{
	  if (BLANK_CHAR(c))
	    continue;
	  if (c == ']')
	    return;
	  stopSection(state, i, c, line);
	  assert(0);
	} //SECT_AFTER_ARG;
    } //for();
  stopSection(state, line.length(), 0, line);
}

void ConfigFile::processValue(const std::string& line)
{
  m_paramValue.erase();
  int state = PARAM_INITIAL;
  std::string name;
  for(std::string::size_type i = 0;i < line.length();i++)
    {
      const char c = line[i];
      //PARAM_INITIAL;
      if (state == PARAM_INITIAL)
	{
	  if (BLANK_CHAR(c))
	    continue;
	  if (c == '#')
	    return;
	  if (validIdentChar(c))
	    {
	      name.erase();
	      name += c;
	      state = PARAM_NAME;
	      continue;
	    }
	  stopParam(state, i, c, line);
	  assert(0);
	} //PARAM_INITIAL;
      if (state == PARAM_NAME)
	{
	  if (validIdentChar(c))
	    {
	      name += c;
	      continue;
	    }
	  if (BLANK_CHAR(c))
	    {
	      m_path.push_back(name);
	      name.erase();
	      state = PARAM_AFTER_NAME;
	      continue;
	    }
	  if (c == '.')
	    {
	      m_path.push_back(name);
		name.erase();
		state = PARAM_NEXT_NAME;
		continue;
	    }
	  stopParam(state, i, c, line);
	  assert(0);
	} //PARAM_NAME;
      //PARAM_NEXT_NAME;
      if (state == PARAM_NEXT_NAME)
	{
	  if (BLANK_CHAR(c))
	    continue;
	  if (validIdentChar(c))
	    {
	      name.erase();
	      name += c;
	      state = PARAM_NAME;
	      continue;
	    }
	  stopParam(state, i, c, line);
	  assert(0);
	} //PARAM_NEXT_NAME;
      //PARAM_AFTER_NAME;
      if (state == PARAM_AFTER_NAME)
	{
	  if (BLANK_CHAR(c))
	    continue;
	  if (c == '.')
	    {
	      state = PARAM_NEXT_NAME;
	      continue;
	    }
	  if (c == '=')
	    {
	      state = PARAM_VALUE;
	      m_assignMode = ModeAssign;
	      continue;
	    }
	  if (c == '+')
	    {
	      if (i + 1 >= line.length())
		break;
	      i++;
	      if (line[i] != '=')
		{
		  stopParam(state, i, line[i], line);
		  assert(0);
		}
	      state = PARAM_VALUE;
	      m_assignMode = ModeAdding;
	      continue;
	    }
	  stopParam(state, i, c, line);
	  assert(0);
	} //PARAM_AFTER_NAME;
      //PARAM_VALUE;
      if (state == PARAM_VALUE)
	{
	  if (c == '#')
	    return;
	  if (c == '\\')
	    {
	      if (i + 1 >= line.length())
		stopParam(state, line.length(), 0, line);
	      m_paramValue += line[i + 1];
	      i++;
	      continue;
	    }
	  m_paramValue += c;
	} //STATE_VALUE;
    }
  if (state != PARAM_VALUE)
    stopParam(state, line.length(), 0, line);
}

void ConfigFile::stopSection(int state,
			     std::string::size_type pos,
			     char ch,
			     const std::string& line)
{
  //FIXME:
  std::cout << "Section error " << state << " at line " << m_linesProcessed + 1 << std::endl;
  std::cout << line << std::endl;
  for(size_t i = 0;i < pos;i++)
    std::cout << " ";
  std::cout << "^" << std::endl;
}

void ConfigFile::stopParam(int state,
			   std::string::size_type pos,
			   char ch,
			   const std::string& line)
{
  //FIXME:
  std::cout << "Parameter error " << state << " at line " << m_linesProcessed + 1 << std::endl;
  std::cout << line << std::endl;
  for(size_t i = 0;i < pos;i++)
    std::cout << " ";
  std::cout << "^" << std::endl;
}

