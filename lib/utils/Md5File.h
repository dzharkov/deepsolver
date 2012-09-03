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

#ifndef DEEPSOLVER_MD5FILE_H
#define DEEPSOLVER_MD5FILE_H

enum {
  Md5FileErrorTooShortLine = 0,
  Md5FileErrorInvalidChecksumFormat = 2
};

/**\brief The exception class for md5file syntax errors
 *
 * This class instance is thrown when md5file syntax error isCon
 * encountered. The client application can access various information
 * with it about the problem like file name, line number,
 * line content and error code.
 *
 * \sa Md5File Md5
 */
class Md5FileException: public DeepsolverException
{
public:
  /**\brief The constructor
   *
   * \param [in] code The error code describing the type of error
   * \param [in] fileName Name of a file being processed
   * \param [in] lineNumber The number of a line with encountered error
   * \param [in] line The text of an invalid line
   */
  Md5FileException(int code,
		      const std::string& fileName,
		      size_t lineNumber,
		      const std::string& line)
    : m_code(code),
      m_fileName(fileName),
      m_lineNumber(lineNumber),
      m_line(line) {}

  /**\brief The destructor*/
  virtual ~Md5FileException() {}

public:
  /**\brief Returns the type of encountered error
   *
   * Use this method to get type of an error.
   *
   * \return The encountered error code
   */
  int getCode() const
  {
    return m_code;
  }

  /**\brief Returns the name of a file with invalid line
   *
   * Use this method to get name of a file with an invalid line.
   *
   * \return Name of a file with an invalid line
   */
  const std::string& getFileName() const
  {
    return m_fileName;
  }

  /**\brief Returns number of an invalid line in file
   *
   * Use this method to get number of line with the syntax problem.
   *
   * \return Number of an invalid line
   */
  size_t getLineNumber() const
  {
    return m_lineNumber;
  }

  /**\brief Returns the text of the invalid line
   *
   * Use this method to get text of the invalid line.
   *
   * \return The content of the invalid line
   */
  const std::string& getLine() const
  {
    return m_line;
  }

  /**\brief Returns the error type
   *
   * This method always returns "md5file" string.
   *
   * \return The type of the an error (always "md5file")
   */
  std::string getType() const
  {
    return "md5file";
  }

  /**\brief Returns the single-line description of the error
   *
   * Use this method to get single-line description (recommended only for debug purposes).
   *
   * \return The single-line error description
   */
  std::string getMessage() const
  {
    return "FIXME:";
  }

private:
  const int m_code;
  const std::string m_fileName;
  const size_t m_lineNumber;
  const std::string m_line;
}; //class Md5FileException;


class Md5File
{
public:
class Item
{
public:
  std::string checksum;
  std::string fileName;
}; //class Item;

public:
  typedef std::vector<Item> ItemVector;
  typedef std::list<Item> ItemList;

public:
  Md5File() {}
  virtual ~Md5File() {}

public:
  void addItemFromFile(const std::string& fileName, const std::string& realPath);
  void loadFromFile(const std::string& fileName);
  void saveToFile(const std::string& fileName) const;
  bool verifyItem(size_t itemIndex, const std::string& fileName) const;

public:
  ItemVector items;
}; //class Md5File;

#endif //DEEPSOLVER_MD5FILE_H;
