
class AttachedRepo
{
public:
  AttachedRepo() {}
  virtual ~AttachedRepo() {}

public:
  void makeId();

public:
  bool operator ==(const AttachedRepo& r) const
  {
    return id == r.id;
  }

  bool operator !=(const AttachedRepo& r) const
  {
    return id != r.id;
  }

public:
  std::string id;
  std::string url;
  std::string component;
  std::string vendorId;
}; //class AttachedRepo;

struct ConfRoot
{
  std::string indexDir;
  std::string indexFetchDir;
  AttachedRepoVector repos;
}; //struct ConfRoot;

