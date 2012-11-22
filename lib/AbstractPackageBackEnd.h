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

/**\brief The abstract interface for every iterator over set of installed packages
 *
 * This class isolates any engine of particular package library which can
 * be used to read database with installed packages data. It instance can
 * be obtained by methods of AbstractPackageBackEnd class.
 *
 * \sa AbstractPackageBackEnd RpmBackEnd RpmInstalledPackagesIterator
 */
class AbstractInstalledPackagesIterator
{
public:
  /**\brief The default constructor*/
  AbstractInstalledPackagesIterator() {}

  /**\brief The destructor*/
  virtual ~AbstractInstalledPackagesIterator() {}

public:
  /**\brief Goes to next installed package and fills data associated with it
   *
   * After creation the iterator is pointed before the first element. Each
   * invocation of this method changes position to next package if there is
   * any and fills necessary data. If there is no next package (including
   * the case there are no packages at all) this method returns zero.
   *
   * \param [out] pkg The Pkg class instance to received data about next installed package
   *
   * \return Non-zero if there is next package or zero otherwise (including the case there are no packages at all)
   */
  virtual bool moveNext(Pkg& pkg) = 0; 
}; //class AbstractInstalledPackagesIterator;

/**\brief The abstract package back-end interface
 *
 * This abstract interface isolates any functions of an particular
 * package library like rpm, dpkg or any other. They can be treated as
 * back-ends for Deepsolver itself.  Nevertheless all methods has strict
 * declaration they semantics can be not exactly the same due to
 * differences in package libraries behaviour. So every developer and
 * user should be very careful on any attempt to add new library
 * support. The first real implementation of this class was librpm so all
 * dependent code has its unpremeditated influence.
 *
 * \sa RpmBackEnd 
 */
class AbstractPackageBackEnd
{
public:
  /**\brief The default constructor*/
  AbstractPackageBackEnd() {}

  /**\brief The destructor*/
  virtual ~AbstractPackageBackEnd() {}

public:
  /**\brief Prepares package back-end for any operations
   *
   * This method should be called before performing any operations with
   * particular package library. For safety reasons it is assumed
   * invocation is needed for every created instance of the back-end but
   * actually it is not always so. For example in case of RpmBackEnd this
   * method may be called only once with any instance.
   */
  virtual void initialize() = 0;

  /**\brief Compares two version strings
   *
   * This method compares two version values and returns an integer value
   * reflecting its relation.
   *
   * \param [in] ver1 The first version value to compare
   * \param [in] ver2 The second version value to compare
   *
   * \return The integer value less than zero in case of ver1 less than ver2, greater than zero if ver1 greater than ver2 and zero otherwise
   */
  virtual int versionCompare(const std::string& ver1, const std::string& ver2 const = 0);

  /**\brief Overlaps two version ranges
   *
   * This method is not symmetric. If second version range has no epoch
   * indication it assumes the same as in first one if there any. So if this
   * method is used for requires processing the require entry should go
   * only as second argument.
   *
   * \param [in] ver1 The first version range to intersect
   * \param [in] ver2 The second version range to intersect
   *
   * \return Non-zero if intersection is not empty
   */
  virtual bool versionOverlap(const VersionCond& ver1, const VersionCond& ver2) const = 0;

  /**\brief Compares two version values for equality
   *
   * This method checks two strings designate same version value. 
   *
   * \param [in] ver1 The first string to compare
   * \param [in] ver2 The second string to compare
   *
   * \return Non-zero if two strings designates the same value or zero otherwise
   */
  virtual bool versionEqual(const std::string& ver1, const std::string& ver2) const = 0;

  /**\brief Checks if one version is newer than another
   *
   * This method checks one version string designates version value newer than another.
   *
   * \param [in] ver1 The first string to compare
   * \param [in] ver2 The second string to compare
   *
   * \return Non-zero if ver1 is greater than ver2 or zero if ver1 is less or equal ver2
   */
  virtual bool versionGreater(const std::string& ver1, const std::string& ver2) const = 0;


  /**\brief Creates an instance of an iterator over the set of installed packages
   *
   * Use this method to get the complete list of packages currently
   * installed in the user system. Since iteration process is very back-end
   * specific the iterator is also provided by abstract interface as
   * back-end object itself.
   *
   * \return The iterator over set of installed packages
   */
  virtual std::auto_ptr<AbstractInstalledPackagesIterator> enumInstalledPackages() = 0;

  /**\brief Reads header information from package file on disk
   *
   * This method reads header data from package file on dist using
   * corresponding functions of an particular package library. Retrieved
   * data is saved in the instance of the universal package class PkgFile. 
   *
   * \param [in] fileName The name of the file to read data from
   * \param [out] pkgFile The object to save retrieved data in
   */
  virtual void readPackageFile(const std::string& fileName, PkgFile& pkgFile) const = 0;

  /**\brief Checks if provided file name is a proper package name
   *
   * Use this method to check package file extension.
   *
   * \param [in] fileName The file name to check
   *
   * \return Non-zero if provided file name is a valid package name
   */
  virtual bool validPkgFileName(const std::string& fileName) const = 0;

  /**\brief Checks if provided file name is a proper source package name
   *
   * Use this method to check source package file extension.
   *
   * \param [in] fileName The file name to check
   *
   * \return Non-zero if provided file name is a valid source package name
   */
  virtual bool validSourcePkgFileName(const std::string& fileName) const = 0;
}; //class AbstractPackageBackEnd;

std::auto_ptr<AbstractPackageBackEnd> createRpmBackEnd();

//General proposal consider this line as main switch what backend we want;
#define CREATE_PACKAGE_BACKEND createRpmBackEnd()

#endif //DEEPSOLVER_ABSTRACT_PACKAGE_BACK_END_H;
