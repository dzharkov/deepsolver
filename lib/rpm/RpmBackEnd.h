/*
   Copyright 2011-2013 ALT Linux
   Copyright 2011-2013 Michael Pozhidaev

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

#ifndef DEEPSOLVER_RPM_BACKEND_H
#define DEEPSOLVER_RPM_BACKEND_Hs

#include"AbstractPackageBackEnd.h"
#include"RpmInstalledPackagesIterator.h"
//FIXME:#include"rpm/Rpmdb.h"
//FIXME:#include"rpm/RpmFile.h"

/**\brief The general interface to librpm functions
 *
 * This class covers all functions of librpm needed for proper Deepsolver
 * work. Since Deepsolver purposed by design as universal package manager
 * all package libraries should be used through an abstraction layer
 * represented by AbstractPackageBackEnd class. Be careful, even with
 * abstraction layer user can face compatibility problems due to
 * differences in details implementation. 
 *
 * \sa AbstractPackageBackEnd
 */

class RpmBackEnd: public AbstractPackageBackEnd
{
public:
  /**\brief The default constructor*/
  RpmBackEnd() {}

  /**\brief The destructor*/
  virtual ~RpmBackEnd() {}

public:
  void initialize();
  int versionCompare(const std::string& ver1, const std::string& ver2) const;
  bool versionOverlap(const VersionCond& ver1, const VersionCond& ver2) const;
  bool versionEqual(const std::string& ver1, const std::string& ver2) const;
  bool versionGreater(const std::string& ver1, const std::string& ver2) const;
  std::auto_ptr<AbstractInstalledPackagesIterator> enumInstalledPackages() const;
  void readPackageFile(const std::string& fileName, PkgFile& pkgFile) const;
  bool validPkgFileName(const std::string& fileName) const;
  bool validSourcePkgFileName(const std::string& fileName) const;
}; //class RpmBackEnd;

#endif //DEEPSOLVER_RPM_BACKEND_H;
