/*
   Copyright 2011-2012 ALT Linux
   Copyright 2011-2012 Dmitry V. Levin
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

#ifndef DEEPSOLVER_DIRECTORY_H
#define DEEPSOLVER_DIRECTORY_H

/**\brief The wrapper for directory operations
 *
 * This class wraps mostly often used operations with directories in
 * files system. Its generally purposed to take care over the error code
 * returned by system calls and throw SystemException in case of
 * problems.
 *
 * \sa File SystemException
 */
class Directory
{
public:
  /**\brief The iterator over the files and subdirectories
   *
   * This class returns all items in some directory. Each item can be
   * examined both as just the name of a file and as the name with parent
   * directories.
   *
   * The iterator is created pointed before the first item. So the user
   * have to call moveNext() method at least once to be able use methods to
   * get name of files.
   */
  class Iterator
  {
  public:
    /**\brief The constructor 
     *
     * This is the main constructor. It takes the name of directory it
     * enumerates and the opened handle to directory. It is strongly
     * recommended to use enumerate() method of Directory class, however the
     * user can create this iterator explicitly.
     *
     * \param [in] path The name of directory used in file names construction
     * \param [in] dir The valid handle to the directory to read data from
     */
    Iterator(const std::string& path, DIR* dir)
      : m_path(path), m_dir(dir) {}

    /**\brief The destructor*/
    virtual ~Iterator()
    {
      if (m_dir)
	closedir(m_dir);
    }

  public:
    /**\brief Moves the iterator to next item 
     *
     * This method moves the iterator to the first item on its first
     * invocation or to the next on each consequent. The method returns zero
     * if next item does not exist. If method returns zero on first call it
     * means the directory is empty.
     *
     * \return Non-zero if next item exists or zero if there is no next item or directory is empty
     */
    bool moveNext();

    /**\brief Returns the name of directory item this iterator is pointing to
     *
     * This method returns the name of a file or directory the iterator is
     * pointing to without path of the parent directory. If method is called
     * before the first invocation of moveNext() or after it has already
     * returned zero this method behaviour is undefined.
     *
     * \return The name of item the iterator is pointing to without parent directory
     */
    std::string getName() const;

    /**\brief Returns the name of directory item this iterator is pointing to with parent directory
     *
     * This method returns the name of a file or directory the iterator is
     * pointing to with path of the parent directory. If method is called
     * before the first invocation of moveNext() or after it has already
     * returned zero this method behaviour is undefined.
     *
     * \return The name of item the iterator is pointing to with parent directory
     */
    std::string getFullPath() const;

  private:
    std::string m_path;
    DIR* m_dir;
    std::string m_currentName;
  }; //class Iterator;

public:
  /**\brief Checks if the directory exists 
   *
   * This method checks if the file system has the required entry and it is
   * a directory by calling stat() system call. It never throes
   * SystemException. This method returns zero even in case the directory
   * is actually present but it existence cannot be checked due to wrong
   * permissions.
   *
   * \return Non-zero if the directory is present and zero otherwise
   */
  static bool isExist(const std::string& path);

  /**\brief Checks if the directory is exist and creates it it otherwise
   *
   * This method checks if the directory is exist and in case it isn't it
   * will be created. The method throws SystemException if there are
   * problems to check the directory or create it.
   */
  static void ensureExists(const std::string& path);

  /**\brief Creates the iterator over the files in the directory
   *
   * This method creates the iterator prepared for enumeration of all files
   * and directories inside of the directory given by path. If the
   * directory is inaccessible for reading the SystemException will be
   * thrown.
   *
   * \param [in] path The name of the directory to enumerate files in
   *
   * \return The auto_ptr<> to the prepared iterator
   */
  static std::auto_ptr<Iterator> enumerate(const std::string& path);
}; //class Directory;

#endif //DEEPSOLVER_DIRECTORY_H;
