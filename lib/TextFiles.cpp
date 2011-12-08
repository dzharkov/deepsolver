

#include"depsolver.h"
#include"TextFiles.h"

#define TEXT_FILE_STOP(x) throw TextFileException(x)

class StdTextFileWriter: public abstractTextFileWriter
{
public:
  StdTextFileWriter()
    : m_opened(0)  {}

  virtual ~StdTextFileWriter() 
  {
    close();
  }

public:
  void open(const std::string& fileName)
  {
    assert(!m_opened);
    assert(!fileName.empty());
    m_stream.open(fileName.c_str());
    if (!m_stream)
      TEXT_FILE_STOP("Cannot open \'" + fileName + "\' for writing");
    m_opened = 1;
  }

  void writeLine(const std::string& line)
  {
    assert(m_opened);
    m_stream << line << std::endl;
  }

  void close()
  {
    if (!m_opened)
      return;
    m_stream.close();
    m_opened = 0;
  }

private:
  bool m_opened;
  std::ofstream m_stream;
}; //class StdTextFileWriter;

class StdTextFileReader: public abstractTextFileReader
{
public:
  StdTextFileReader()
    : m_opened(0)  {}

  virtual ~StdTextFileReader() 
  {
    close();
  }

public:
  void open(const std::string& fileName)
  {
    assert(!m_opened);
    assert(!m_fileName.empty());
    m_stream.open(fileName.c_str());
    if (!m_stream)
      TEXT_FILE_STOP("Cannot open \'" + fileName + "\' for reading");
    m_opened = 1;
  }

  bool readLine(std::string& line)
  {
    assert(m_opened);
    std::getline(m_stream, line);
    if (!m_stream)
      {
	close();//Immediate closing of input stream, but user must not know about it;
	return 0;
      }
    return 1;
  }

  void close()
  {
    if (!m_opened)
      return;
    m_stream.close();
    m_opened = 0;
  }

private:
  bool m_opened;
  std::ifstream m_stream;
}; //class StdTextFileReader;

std::auto_ptr<AbstractTextFileReader> createTextFileReader(int type, const std::string& fileName)
{
}

std::auto_ptr<AbstractTextFileWriter> createTextFileWriter(int type, const std::string& fileName)
{
}
