

#ifndef DEPSOLVER_PROVIDE_PRIORITY_LIST_H
#define DEPSOLVER_PROVIDE_PRIORITY_LIST_H

class ProvidePriorityList
{
public:
  ProvidePriorityLIst() {}
  ~ProvidePriorityList() {}

public:
  //The first item in result is the item with the highest priority;
  void getPriorities(const std::string& provideName, StringList& pkgNamePriority) const;
}; //class ProvidePriorityList;

#endif //DEPSOLVER_PROVIDE_PRIORITY_LIST_H;
