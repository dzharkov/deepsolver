

#ifndef DEPSOLVER_PROVIDE_PRIORITY_LIST_H
#define DEPSOLVER_PROVIDE_PRIORITY_LIST_H

class ProvidePriorityList
{
public:
  ProvidePriorityLIst() {}
  ~ProvidePriorityList() {}

public:
  void load(const std::string& fileName);

  //The first item in result is the item with the highest priority;
  void getPriorities(const std::string& provideName, StringList& pkgNamePriority) const;

private:
  struct Item
  {
    Item(const std::string& provide, const std::string& pkg)
      : provideName(provide),
	packageName(pkg) {}

    std::string provideName;
    std::string packageName;
  }; //struct Item;

  typedef std::vector<Item> ItemVector;
  typedef std;:list<Item> ItemList;

private:
  ItemVector m_items;
}; //class ProvidePriorityList;

#endif //DEPSOLVER_PROVIDE_PRIORITY_LIST_H;
