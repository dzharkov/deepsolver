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

#ifndef DEEPSOLVER_FILE_H
#define DEEPSOLVER_FILE_H

/**\brief The wrapper for file operations
 *
 * This class wraps mostly often used operations with files. It is
 * generally purposed to take care over the error code
 * returned by system calls and throw SystemException in case of
 * problems.
 *
 * \sa Directory SystemException
 */
class File
{
public:
  /**\brief The default constructor*/
  File()
    : m_fd(-1) {}

  /**\brief The destructor*/
  virtual ~File()
  {
    close();
  }

  /**\brief Opens existing file in read/write mode
   *
   * This method opens an existing file and associates the current object
   * with handle of opened file. In case the ::open() system call had
   * returned an error value the SystemException is thrown with
   * corresponding information.
   *
   * \param [in] fileName The name of file to open
   */ 
  void open(const std::string fileName);

  /**\brief Opens existing file in read-only mode
   *
   * This method opens an existing file in read-only mode and associates the current object
   * with handle of opened file. In case the ::open() system call had
   * returned an error value the SystemException is thrown with
   * corresponding information.
   *
   * \param [in] fileName The name of file to open
   */ 
  void openReadOnly(const std::string& fileName);

  /**\brief Creates new file in file system
   *
   * This method creates new file with specified name. The directory the
   * new file should be placed in must exist. The new file is created with
   * flags O_RDWR | O_CREAT | O_TRUNC and 0666 mode. If something is wrong and new file cannot be
   * created the SystemException is thrown with corresponding information.
   *
   * \param [in] fileName The name of a file to create
   */
  void create(const std::string& fileName);

  /**\brief Closes previously opened file
   *
   * This method closes previously opened file. If file is already closed
   * or never be opened it is not an error, in this case nothing is
   * done. This method is always called automatically on object
   * destruction.
   */
  void close();

  /**\brief Reads data from an opened file
   *
   * Length of the buffer is not limited. In case of large buffer this
   * method produces several subsequent calls of corresponding system
   * function. In addition this method takes care to be sure the operation
   * is performed completely until entire requested data is read or error
   * occurred.
   *
   * \param [out] buf The buffer to save read data to
   * \param [in] bufSize The size of provided buffer
   *
   * \return The number of read bytes, can be less then bufSize only in case of reading near the end of file
   */
    size_t read(void* buf, size_t bufSize);

  /**\brief Writes data to an opened file
   *
   * Length of the buffer is not limited. In case of large buffer this
   * method produces several subsequent calls of corresponding system
   * function. In addition this method takes care to be sure the operation
   * is performed completely until entire buffer content is written or error
   * occurred.
   *
   * \param [in] buf The buffer with the data  to write
   * \param [in] bufSize The size of provided data
   *
   * \return The number of successfully written bytes
   */
    size_t write(const void* buf, size_t bufSize);

  /**\brief Returns the handle of an opened file
   *
   * This method returns the handle of opened file provided by operating
   * system. It should never be called for closed file or for files never
   * be opened.
   *
   * \return The handle of opened file
   */
  int getFd() const
  {
    assert(m_fd >= 0);
    return m_fd;
  }

  /**\brief Returns the file name without parent directories
   *
   * This method returns the provided string from the last slash character
   * until the its end or the string itself if there are no slashes.
   *
   * \param [in] The file name to take base part from
   *
   * \return The file name without parent directories
   */
  static std::string baseName(const std::string& fileName);

  /**\brief Removes the file or remove the hard link if there are more than one
   *
   * This method removes the hard link to the file. Usually the file has
   * only one hard link and it means the file will be removed at all.
   *
   * \param [in] fileName The name of the file to remove hard link to
   */
  static void unlink(const std::string& fileName);

  /**\brief Checks if file is a regular file
   *
   * FIXME
   */
  static bool isRegFile(const std::string& fileName);

  /**\brief Checks if file is a directory
   *
   * FIXME
   */
    static bool isDir(const std::string& fileName);

  /**\brief Checks if file is a symlink
   *
   * FIXME
   */
  static bool isSymLink(const std::string& fileName);

  /**\brief FIXME*/
  void readAhead(const std::string& fileName);

protected:
  int m_fd;
}; //class File;

#endif //DEEPSOLVER_FILE_H;
