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

#ifndef DEEPSOLVER_ABSTRACT_PACKAGE_BACK_END_H
#define DEEPSOLVER_ABSTRACT_PACKAGE_BACK_END_H

#include"Pkg.h"

class AbstractInstalledPackagesIterator
{
public:
  AbstractInstalledPackagesIterator() {}
  virtual ~AbstractInstalledPackagesIterator() {}

public:
  virtual bool moveNext(Pkg& pkg) = 0; 
}; //class AbstractInstalledPackagesIterator;

class AbstractPackageBackEnd
{
public:
  AbstractPackageBackEnd() {}
  virtual ~AbstractPackageBackEnd() {}

public:
  virtual int versionCompare(const std::string& ver1, const std::string& ver2 const = 0);

  /**\brief Overlaps two version ranges
   *
   * This function is not symmetric. If second version range has no epoch
   * value it assumes the same as in first one if there any. So if this
   * function is used for requires processing the require entry should go
   * only as second argument.
   *
   * \param [in] ver1 The first version range to intersect
   * \param [in] ver2 The second version range to intersect
   *
   * \return Non-zero if intersection is not empty
   */
  virtual bool versionOverlap(const VersionCond& ver1, const VersionCond& ver2) const = 0;

  virtual bool versionEqual(const std::string& ver1, const std::string& ver2) const = 0;

  /**\brief Checks if one version is newer than another
   *
   * \return /Non-zero if ver1 greater than ver2 or zero if ver1 less or equals ver2
   */
  virtual bool versionGreater(const std::string& ver1, const std::string& ver2) const = 0;

  virtual std::auto_ptr<AbstractInstalledPackagesIterator> enumInstalledPackages() = 0;
  virtual void readPackageFile(const std::string& fileName, PkgFile& pkgFile) = 0;
  virtual bool validPkgFileName(const std::string& fileName) const = 0;
  virtual bool validSourcePkgFileName(const std::string& fileName) const = 0;
}; //class AbstractPackageBackEnd;

std::auto_ptr<AbstractPackageBackEnd> createRpmBackEnd();

//General proposal consider this line as main switch what backend we want;
#define CREATE_PACKAGE_BACKEND createRpmBackEnd()

#endif //DEEPSOLVER_ABSTRACT_PACKAGE_BACK_END_H;
