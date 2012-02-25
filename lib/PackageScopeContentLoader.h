
#ifndef DEEPSOLVER_PACKAGE_SCOPE_CONTENT_LOADER_H
#define DEEPSOLVER_PACKAGE_SCOPE_CONTENT_LOADER_H

#include"PackageScopeContent.h"

class PackageScopeContentLoader: public PackageScopeContent
{
public:
  /**\brief The default constructor*/
  PackageScopeContentLoader() 
    : m_stringBuf(NULL) {}

  /**\brief The destructor*/
  virtual ~PackageScopeContentLoader() 
  {
    if (m_stringBuf != NULL)
      delete[] m_stringBuf;
  }

public:
  void loadFromFile(const std::string& fileName);

private:
  void readNames(std::ifstream& s, size_t namesBufSize);

private:
  char* m_stringBuf;
}; //class PackageScopeContentLoader;

#endif //DEEPSOLVER_PACKAGE_SCOPE_CONTENT_LOADER_H;
