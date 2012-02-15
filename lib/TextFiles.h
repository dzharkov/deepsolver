
#ifndef DEEPSOLVER_TEXT_FILES_H
#define DEEPSOLVER_TEXT_FILES_H

#include"DeepsolverException.h"

enum {
  TextFileStd = 0,
  TextFileGZip = 1
};

class TextFileException: public DeepsolverException
{
public:
  TextFileException(const std::string& msg)
    : m_msg(msg) {}

  virtual ~TextFileException() {}

public:
  std::string getType() const
  {
    return "textfile";
  }

  std::string getMessage() const
  {
    return m_msg;
  }

private:
  std::string m_msg;
}; //class TextFileException;

class AbstractTextFileWriter
{
public:
  virtual ~AbstractTextFileWriter() {}

public:
  virtual void writeLine(const std::string& line) = 0;
  virtual void close() = 0;
}; //class AbstractTextFileWriter;

class AbstractTextFileReader
{
public:
  virtual ~AbstractTextFileReader() {}

public:
  virtual bool readLine(std::string& line) = 0;
  virtual void close() = 0;
}; //class AbstractTextFileReader;

std::auto_ptr<AbstractTextFileReader> createTextFileReader(int type, const std::string& fileName);
std::auto_ptr<AbstractTextFileWriter> createTextFileWriter(int type, const std::string& fileName);

#endif //DEEPSOLVER_TEXT_FILES_H;
