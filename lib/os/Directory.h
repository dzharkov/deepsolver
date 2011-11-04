
#ifndef FIXME_DIRECTORY_H
#define FIXME_DIRECTORY_H

class Directory
{
public:
  static bool isExist(const std::string& path);
  static void ensureExists(const std::string& path);
}; //class Directory;

#endif //FIXME_DIRECTORY_H;
