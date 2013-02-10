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

#ifndef DEEPSOLVER_DUMMY_PACKAGE_BACK_END_H
#define DEEPSOLVER_DUMMY_PACKAGE_BACK_END_H

class DummyInstalledPackagesIterator: public AbstractInstalledPackagesIterator
{
public:
  /**\brief The default constructor*/
  DummyInstalledPackagesIterator() {}

  /**\brief The destructor*/
  virtual ~DummyInstalledPackagesIterator() {}

public:
  bool moveNext(Pkg& pkg)
  {
    return false;
  }
}; //class DummyInstalledPackagesIterator;

class DummyPackageBackEnd: public AbstractPackageBackEnd
{
public:
  /**\brief The default constructor*/
  DummyPackageBackEnd() {}

  /**\brief The destructor*/
  virtual ~AbstractPackageBackEnd() {}

public:
  void initialize()
  {
    //Do nothing here;
  }

  int versionCompare(const std::string& ver1, const std::string& ver2) const
  {
    if (ver1 < ver2)
      return -1;
    if (ver1 > ver2)
      return 1;
    return 0;
  }

  bool versionOverlap(const VersionCond& ver1, const VersionCond& ver2) const
  {
    return 0;
  }

  bool versionEqual(const std::string& ver1, const std::string& ver2) const
  {
    return ver1 == ver2;
  }

  bool versionGreater(const std::string& ver1, const std::string& ver2) const
  {
    return ver1 > ver2;
  }

  std::auto_ptr<AbstractInstalledPackagesIterator> enumInstalledPackages() const
  {
    return std::auto_ptr<AbstractInstalledPackagesIterator>(new DummyInstalledPackagesIterator());
  }

  void readPackageFile(const std::string& fileName, PkgFile& pkgFile) const
  {
    //Do nothing here;
    pkgFile = PkgFile();
  }

  bool validPkgFileName(const std::string& fileName) const
  {
    return 1;
  }

  bool validSourcePkgFileName(const std::string& fileName) const
  {
    return 1;
  }
}; //class AbstractPackageBackEnd;

#endif //DEEPSOLVER_DUMMY_PACKAGE_BACK_END_H;
