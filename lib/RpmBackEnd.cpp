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

#include"deepsolver.h"
#include"AbstractPackageBackEnd.h"
#include"rpm/Rpmdb.h"
#include"rpm/RpmFile.h"

class RpmInstalledPackagesIterator: public AbstractInstalledPackagesIterator
{
public:
  RpmInstalledPackagesIterator() {}
  virtual ~RpmInstalledPackagesIterator() {}

public:
  void init()
  {
    m_rpmdb.openEnum();
  }

  bool moveNext(Pkg& pkg)
  {
    if (m_rpmdb.moveNext(pkg))
      return 1;
    m_rpmdb.close();
    return 0;
  }

private:
  Rpmdb m_rpmdb;
}; //class RpmInstalledPackagesIterator;

class RpmBackEnd: public AbstractPackageBackEnd
{
public:
  RpmBackEnd() {}
  virtual ~RpmBackEnd() {}

public:
  std::auto_ptr<AbstractInstalledPackagesIterator> enumInstalledPackages()
  {

    std::auto_ptr<RpmInstalledPackagesIterator> rpmIterator(new RpmInstalledPackagesIterator());
    rpmIterator->init();
    std::auto_ptr<AbstractInstalledPackagesIterator> it(rpmIterator.get());
    rpmIterator.release();
    return it;
  }

void readPackageFile(const std::string& fileName, PkgFile& pkgFile)
  {
    readRpmPkgFile(fileName, pkgFile);
  }

  bool validPkgFileName(const std::string& fileName) const
  {
    if (fileName.length() <= 4)
      return 0;
    const std::string ext = ".rpm";
    for(std::string::size_type i = 0;i < ext.size();i++)
      if (fileName[fileName.length() - ext.length() + i] != ext[i])
	return 0;
    return 1;
  }

  bool validSourcePkgFileName(const std::string& fileName) const
  {
    if (fileName.length() <= 8)
      return 0;
    const std::string ext = ".src.rpm";
    for(std::string::size_type i = 0;i < ext.size();i++)
      if (fileName[fileName.length() - ext.length() + i] != ext[i])
	return 0;
    return 1;
  }


}; //class RpmBackEnd;

std::auto_ptr<AbstractPackageBackEnd> createRpmBackEnd()
{
  return std::auto_ptr<AbstractPackageBackEnd>(new RpmBackEnd());
}
