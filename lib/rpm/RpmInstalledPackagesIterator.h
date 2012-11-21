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

#ifndef DEEPSOLVER_RPM_INSTALLED_PACKAGES_ITERATOR_H
#define DEEPSOLVER_RPM_INSTALLED_PACKAGES_ITERATOR_H

#include"AbstractInstalledPackagesIterator.h"
#include<rpm/rpmlib.h>

/**\brief The iterator over set of installed packages
 *
 * This iterator reads the rpmdb database and provides information about
 * each installed package.  This class instance should not be created
 * directly, use methods of AbstractPackageBackEnd class.
 *
 * \sa AbstractPackageBackEnd RpmBackEnd
 */
class RpmInstalledPackagesIterator: public AbstractInstalledPackagesIterator
{
public:
  /**\brief The default constructor*/
  RpmInstalledPackagesIterator() {}

  /**\brief The destructor*/
  virtual ~RpmInstalledPackagesIterator() {}

public:
  void openEnum();
  bool moveNext(Pkg& pkg);

private:
  rpmdb m_db;
  rpmdbMatchIterator m_it;
}; //class RpmInstalledPackagesIterator;

#endif //DEEPSOLVER_RPM_INSTALLED_PACKAGES_ITERATOR_H;
