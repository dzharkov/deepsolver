/*
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 *
 * Changed so as no longer to depend on Colin Plumb's `usual.h' header
 * definitions; now uses stuff from dpkg's config.h.
 *  - Ian Jackson <ian@chiark.greenend.org.uk>.
 * Still in the public domain.
 */

#ifndef DEEPSOLVER_MD5FILE_H
#define DEEPSOLVER_MD5FILE_H

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
  void saveToFile(const std::string& fileName);
  void verifyItem(const std::string& fileName);

public:
  ItemVector m_items;
}; //class Md5File;

#endif //DEEPSOLVER_MD5FILE_H;
