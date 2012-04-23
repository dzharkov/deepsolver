
#ifndef DEEPSOLVER_CONFIG_DATA_H
#define DEEPSOLVER_CONFIG_DATA_H

struct ConfRepo
{
  ConfRepo()
    : enabled(1) {}

  ConfRepo(const std::string& n)
    : name(n), enabled(1) {}

  std::string name;
  bool enabled;
  std::string url;
  StringVector components; 
  std::string vendor;
}; //struct ConfRepo;

typedef std::vector<ConfRepo> ConfRepoVector;

struct ConfRoot
{
  std::string indexDir;
  std::string indexFetchDir;
  ConfRepoVector repo;
}; //struct ConfRoot;

#endif //DEEPSOLVER_CONFIG_DATA_H;
