/*
   Copyright 2011-2012 ALT Linux
   Copyright 2011-2012 Michael Pozhidaev

   This file is part of the Deepsolver.

   Deepsolver is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   Deepsolver is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
*/

#ifndef DEEPSOLVER_PACKAGE_SCOPE_CONTENT_BUILDER_H
#define DEEPSOLVER_PACKAGE_SCOPE_CONTENT_BUILDER_H

#include"Pkg.h"
#include"AbstractPackageRecipient.h"

class PackageScopeContentBuilder: public AbstractPackageRecipient
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

  void onNewPkgFile(const PkgFile& pkgFile);
  void commit();

private:
  struct RelInfo
  {
    RelInfo()
      : pkgId(BAD_PACKAGE_ID), type(0), ver(NULL)  {}

    PackageId pkgId;
    VerDirection type;
    char* ver;
  }; //struct RelINfo;

  typedef std::list<RelInfo> RelInfoList;
  typedef std::vector<RelInfo> RelInfoVector;

  struct PkgInfo
  {
    PkgInfo()
      : pkgId(BAD_PACKAGE_ID),
	epoch(0),
	ver(NULL),
	release(NULL),
	buildTime(0),
	requiresPos(0), requiresCount(0),
	providesPos(0), providesCount(0),
      conflictsPos(0), conflictsCount(0),
      obsoletesPos(0), obsoletesCount(0) {}

    bool operator <(const PkgInfo& pkgInfo) const
    {
      return pkgId < pkgInfo.pkgId;
    }

    bool operator >(const PkgInfo& pkgInfo) const
    {
      return pkgId > pkgInfo.pkgId;
    }

    PackageId pkgId;
    Epoch epoch;
    char* ver;
    char* release;
    time_t buildTime;
    size_t requiresPos, requiresCount;
    size_t providesPos, providesCount;
    size_t conflictsPos, conflictsCount;
    size_t obsoletesPos, obsoletesCount;
    size_t aux;
  }; //struct PkgInfo;

  typedef std::list<PkgInfo> PkgInfoList;
  typedef std::vector<PkgInfo> PkgInfoVector;


private:
  void processRels(const NamedPkgRelVector& rels, size_t& pos, size_t& count);
  PackageId registerName(const std::string& name);
  void freeStringValues();

private:
  typedef std::vector<char*> StringValueVector;

private:
  typedef std::map<std::string, PackageId> NameToPackageIdMap;

private:
  StringVector m_names;
  NameToPackageIdMap m_namesToId;
  PkgInfoVector m_pkgInfoVector;
  RelInfoVector m_relInfoVector;
  //Just for saving;
  StringValueVector m_stringValues;
    SizeVector m_relVerIndices;
}; //class PackageScopeContentBuilder;

#endif //DEEPSOLVER_PACKAGE_SCOPE_CONTENT_BUILDER_H;
