
#include "deepsolver.h"
#include"ConfigFile.h"

class ConfigFile::SyntaxItem
{
public:
  enum {
    Ident = 0,
    String = 1,
    Number = 2,
    OpenBracket = 3,
    CloseBracket = 4,
    Equals = 5
  };

public:
  SyntaxItem() 
    : type(Ident), intValue(0) {}

  SyntaxItem(int t)
    : type(t), intValue(0) {}

  SyntaxItem(int t, const std::string& value)
    : type(t), intValue(0), stringValue(value) {}

  SyntaxItem(int value)
    : type(Number), intValue(value) {}

public:
  int type;
  int intValue;
  std::string stringValue;
}; //class SyntaxItem;


void ConfigFile::read(const std::string& fileName)
{
  //FIXME:
}

void ConfigFile::processSyntaxItems(const SyntaxItemVector& items)
{
  //FIXME:
}
