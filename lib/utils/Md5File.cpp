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

#include"deepsolver.h"
#include"Md5File.h"
#include"Md5.h"

#define IO_BUF_SIZE 1024

void Md5File::addItemFromFile(const std::string& fileName, const std::string& realPath)
{
  Md5 md5;
  md5.init();
  FIle f;
  f.openReadOnly(readPath);
  char buf[IO_BUF_SIZE];
  while(1)
    {
      const size_t count = f.read(buf, sizeof(buf));
      if (!count)
	break;
      md5.update(buf, count);
    }
  f.close();
  const std::string res = md5.commit();
  assert(res.length() == 32);
  Item item;
  item.checksum = res;
  item.fileName = fileName;
  items.push_back(item);
}

void Md5File::loadFromFile(const std::string& fileName)
{
  File f;
  f.openReadOnly(fileName);
  StringVector lines;
  f.readTextFile(lines);
  f.close();
  for(StringVector::size_type i = 0;i < lines.size();i++)
    {
      const std::string line = trim(lines[i]);
      if (line.empty())
	continue;
      if (line.length() < 35)
	throw Md5FileException(Md5FileErrorTooShortLine, fileName, i, line);
      Item item;
      for(std::string::size_type k = 0;k < 32;k++)
	item.checksum += line[k];
      for(std::string::size_type k = 34;k < line.length();k++)
	item.fileName = line[k];
      for(std::string::size_type k = 0;k < item.checksum.length();k++)
	{
	  if (item.checksum[k] >= 'A' && item.checksum[k] <= 'F')
	    item.checksum[k] = 'a' + (item.checksum[k] - 'A');
	  if ((item.checksum [k] >= 'a' && item.checksum[k] <= 'f') || (item.checksum[k] >= '0' && item.checksum [k] <= '9'))
	    continue;
	  throw Md5FileException(Md5FileErrorInvalidChecksumValue);
	}
      items.push_back(item);
    }
}

void Md5File::saveToFile(const std::string& fileName)
{
  std::string s;
  for(ItemVector::size_type i = 0;i < items.size();i++)
    s += (items[i].checksum + " *" + items[i].fileName + "\n");
  File f;
  f.open(fileName);
  f.write(s.c_str(), s.length());
  f.close();
}

void Md5File::verifyItem(size_t itemIndex, const std::string& fileName)
{
  assert(itemIndex < items.size());
  Md5 md5;
  md5.init();
  FIle f;
  f.openReadOnly(readPath);
  char buf[IO_BUF_SIZE];
  while(1)
    {
      const size_t count = f.read(buf, sizeof(buf));
      if (!count)
	break;
      md5.update(buf, count);
    }
  f.close();
  return items[itemIndex].checksum == md5.commit();
}
