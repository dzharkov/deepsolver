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

#include"deepsolver.h"
#include<rpm/rpmlib.h>

int rpmVerCmp(const std::string& ver1, const std::string& ver2)
{
  return rpmvercmp(ver1.c_str(), ver2.c_str());
}

static int buildSenseFlags(const VersionCond& c)
{
  int value = 0;
  if (c.isEqual())
    value |= RPMSENSE_EQUAL;
  if (c.isLess())
    value |= RPMSENSE_LESS;
  if (c.isGreater())
    value |= RPMSENSE_GREATER;
  return value;
}

int rpmVerOverlap(const VersionCond& v1, const VersionCond& v2)
{
  return rpmRangesOverlap("", v1.version.c_str(), buildSenseFlags(v1),
			  "", v2.version.c_str(), buildSenseFlags(v2));
}


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

int versionCompare(const std::string& ver1, const std::string& ver2)
{
  return rpmVerCmp(ver1, ver2);
}

bool versionOverlap(const VersionCond& ver1, const VersionCond& ver2)
{
  return rpmVerOverlap(ver1, ver2);
}

bool versionEqual(const std::string& ver1, const std::string& ver2)
{
  return versionOverlap(VersionCond(ver1), VersionCond(ver2));
}

bool versionGreater(const std::string& ver1, const std::string& ver2)
{
  return versionOverlap(VersionCond(ver1, VerLess), VersionCond(ver2));
}
