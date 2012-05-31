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

#ifndef DEEPSOLVER_PACKAGE_SCOPE_CONTENT_H
#define DEEPSOLVER_PACKAGE_SCOPE_CONTENT_H

#include"Pkg.h"

/**\brief The information about packages from available repositories
 *
 * This class contains information about available package list with
 * their attributes required for dependent package set calculation. It
 * has several methods for stored data access but it is not able to load
 * or fill by the other way stored information. This class is not
 * abstract, however every user has to use one of the child classes. One
 * of them is prepared for loading content from the binary file on disk
 * and the another one is purposed for converting data from the
 * repository index and saving it to binary file.
 *
 * \sa PackageScopeContentBuilder PackageScopeContentLoader
 */
class PackageScopeContent
{
public:
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

  struct ProvideMapItem
  {
    ProvideMapItem()
      : provideId(BAD_PACKAGE_ID), pkgId(BAD_PACKAGE_ID) {}

    ProvideMapItem(PackageId prId, PackageId pkId)
      : provideId(prId), pkgId(pkId) {}

    bool operator <(const ProvideMapItem& item) const
    {
      return provideId < item.provideId;
    }

    bool operator >(const ProvideMapItem& item) const
    {
      return provideId > item.provideId;
    }

    PackageId provideId, pkgId;
  }; //struct ProvideMapItem;

  typedef std::vector<ProvideMapItem> ProvideMapItemVector;
  typedef std::list<ProvideMapItem> ProvideMapItemList;

public:
  /**\brief The default constructor*/
  PackageScopeContent() {}

  /**\brief The destructor*/
  virtual ~PackageScopeContent() {}

public:
  static void locateRange(const PkgInfoVector& pkgs, PackageId packageId, VarId& fromPos, VarId& toPos );

public:
  void getProviders(PackageId provideId, PackageIdVector& providers) const;
  bool checkName(const std::string& name) const;
  PackageId strToPackageId(const std::string& name) const;
  std::string packageIdToStr(PackageId packageId) const;
  const PkgInfoVector& getPkgs() const;
  const RelInfoVector& getRels() const;
  void addStringToAutoRelease(char* str);

public:
  StringVector names;
  PkgInfoVector pkgInfoVector;
  RelInfoVector relInfoVector;
  ProvideMapItemVector provideMap;
}; //class PackageScopeContent;

#endif //DEEPSOLVER_PACKAGE_SCOPE_CONTENT_H;
