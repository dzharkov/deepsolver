
struct PrioritySortItem
{
  PrioritySortItem(const AbstractPackageBackEnd& b,
		   VarId v,
		   const std::string& n)
    : backEnd(b),
      varId(v),
      name(n) {}

  bool operator <(const PrioritySortItem& item) const
  {
    return backEnd.versionCompare(name, item.name) < 0;
  }

  bool operator >(const PrioritySortItem& item) const
  {
    return backEnd.versionCompare(name, item.name) > 0;
  }

  const AbstractPackageBackEnd& backEnd;
  VarId varId;
  std::string name;
}; //struct PrioritySortItem;

typedef std::vector<PrioritySortItem> PrioritySortItemVector;
typedef std::list<PrioritySortItem> PrioritySortItemList;

