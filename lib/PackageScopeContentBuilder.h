
#ifndef DEEPSOLVER_PACKAGE_SCOPE_CONTENT_BUILDER_H
#define DEEPSOLVER_PACKAGE_SCOPE_CONTENT_BUILDER_H

#include"PackageScopeContent.h"

class PackageScopeContentBuilder: public PackageScopeContent
{
public:
  PackageScopeContentBuilder() {}
  virtual ~PackageScopeContentBuilder() 
  {
    freeStringValues();
  }

public:
  /**\brief Saves prepared package scope content to binary file on disk
   *
   * This method can perform complete saving of all internal data to file
   * on disk for further loading with PackageScopeContentLoader
   * class. Since data is saved in binary format the created file is not
   * protected from different binary data representation on various
   * machines (little endian vs big endian) and is suited only for using on
   * the same computer it was saved.
   * \param [in] fileName The name of the file to save data to
   */
  void saveToFile(const std::string& fileName) const;

  void addPkg(const PkgFile& pkgFile);
  void addProvideMapItem(const std::string& provideName, const std::string& packageName);
  void commit();

private:
  void processRels(const NamedPkgRelVector& rels, size_t& pos, size_t& count);
  PackageId registerName(const std::string& name);
  void freeStringValues();

private:
  typedef std::vector<char*> StringValueVector;

private:
  StringValueVector m_stringValues;
  SizeVector m_pkgVerIndices, m_relVerIndices;
}; //class PackageScopeContentBuilder;

#endif //DEEPSOLVER_PACKAGE_SCOPE_CONTENT_BUILDER_H;
