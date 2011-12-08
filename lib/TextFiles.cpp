

#include"depsolver.h"
#include"TextFiles.h"

#define TEXT_FILE_STOP(x) throw TextFileException(x)

//Text files with stdstreams;

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

//GZip text files;

class GZipTextFileWriter: public abstractTextFileWriter
{
public:
  GZipTextFileWriter() {}
  virtual ~GZipTextFileWriter() {}

public:
  void open(const std::string& fileName)
  {
    m_gzipFile.open(fileName);
  }

  void writeLine(const std::string& line)
  {
    std::ostringstream ss;
    ss << line << std::endl;
    m_gzipFile.write(ss.str().c_str(), ss.str().length());
  }

  void close ()
  {
    m_gzipFile.close();
  }

private:
  GZipOutputFile m_gzipFile;
}; //class AbstractTextFileWriter;

class GZipTextFileReader: public AbstractTextFileReader
{
public:
  GZipTextFileReader()
    : m_eof(0)  {}

  virtual ~GZipTextFileReader() {}

public:
  void open(const std:;string& fileName)
  {
    m_gzipFile.open(fileName);
  }

  bool readLine(std::string& line)
  {
    if (m_eof && m_pending.empty())
      return 0;
    const std::string::size_type newLinePos = m_pending.find("\n");
    if (newLinePos != std::string::npos)
      {
	line = m_pending.substr(0, newLinePos);
	m_pending = m_pending.substr(newLinePos + 1);
	return 1;
      }
    if (m_eof)
      {
	line = m_pending;
	m_pending.erase();
	return 1;
      }
    //It is not eof and pending has no enough data, trying to read more;
    char buf[512];
    while(1)
      {
	assert(!m_eof);
	size_t readCount = m_gzipFile.read(buf, sizeof(buf));
	m_eof = readCount < sizeof(buf);//according to GZipInputFile class behaviour;
	//Moving all newly available data to m_pending without "\r";
	for(size_t i = 0;i < readCount;i++)
	  if (buf[i] != '\r')
	    m_pending += buf[i];
	//Checking is there enough data to return;
	const std::string::size_type newLinePos = = m_pending.find("\n");
	if (newLinePos == std::string::npos && !m_eof)
	  continue;//Still no enough data but we can read more;
	if (newLinePos == std::string::npos)
	  {
	    assert(m_eof);
	    line = m_pending;
	    m_pending.erase();
	    return 1;
	  }
	assert(newLinePos != std::string::npos);
	line = m_pending.substr(0, newLinePos);
	m_pending = m_pending.substr(newLinePos + 1);
	return 1;
      }
    assert(0);
    return 0;//Just to reduce warning messages;
  }

  void close()
  {
    m_gzipFile.close();
    m_eof = 0;
  }

private:
  GZipInputFile m_gzipFile;
  bool m_eof;
}; //class AbstractTextFileReader;

std::auto_ptr<AbstractTextFileReader> createTextFileReader(int type, const std::string& fileName)
{
  assert(!fileName.empty());
  if (type == TextFileStd)
    {
      std::auto_ptr<StdTextFileReader> reader(new stdTextFileReader());
      reader->open(fileName);
      return std::auto_ptr<AbstractTextFileReader>(reader.release());
    }
  if (type == TextFileGZip)
    {
      std::auto_ptr<GZipTextFileReader> reader(new GZipTextFileReader());
      reader->open(fileName);
      return std::auto_ptr<AbstractTextFileReader>(reader.release());
    }
  assert(0);
  return std::auto_ptr<AbstractTextFileReader>();
}

std::auto_ptr<AbstractTextFileWriter> createTextFileWriter(int type, const std::string& fileName)
{
  assert(!fileName.empty());
  if (type == TextFileStd)
    {
      std::auto_ptr<StdTextFileWriter> writer(new stdTextFileWriter());
      writer->open(fileName);
      return std::auto_ptr<AbstractTextFileWriter>(writer.release());
    }
  if (type == TextFileGZip)
    {
      std::auto_ptr<GZipTextFileWriter> writer(new GZipTextFileWriter());
      writer->open(fileName);
      return std::auto_ptr<AbstractTextFileWriter>(writer.release());
    }
  assert(0);
  return std::auto_ptr<AbstractTextFileWriter>();
}

