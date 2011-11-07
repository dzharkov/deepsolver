
#ifndef FIXME_DIRECTORY_H
#define FIXME_DIRECTORY_H

class Directory
{
public:
  class Iterator
  {
  public:
    Iterator(const std::string& path, DIR* dir)
      : m_path(path), m_dir(dir) {}

    virtual ~Iterator()
    {
      if (m_dir)
	closedir(m_dir);
    }

  public:
    bool moveNext();
    std::string getName() const;
    std::string getFullPath() const;

  private:
    std::string m_path;
    DIR* m_dir;
    std::string m_currentName;
  }; //class Iterator;

public:
  static bool isExist(const std::string& path);
  static void ensureExists(const std::string& path);
  static std::auto_ptr<Iterator> enumerate(const std::string& path);
}; //class Directory;

#endif //FIXME_DIRECTORY_H;
