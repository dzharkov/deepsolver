

#include<assert.h>
#include<string>
#include<map>
#include"Package.h"
#include"SATBase.h"

typedef size_t PackageId;
typedef std::vector<PackageId> PackageIdVector;
#define BAD_PACKAGE_ID (PackageId)-1

struct ProvideIndex
{
  ProvideIndex(): pos(0), count(0) {}
  ProvideIndex(size_t p, size_t c): pos(p), count(c)  {}

  size_t pos, count; 
}; //struct ProvideIndex;

typedef std::map<std::string, PackageId> PackageToIdMap;
typedef std::map<std::string, ProvideIndex} ProvideIndexMap;

void fillSAT(const PackageVector& packages, PackageId, forPackage, SAT& sat)
{
  PackageToIdMap revMap;
  for(PackageVector::size_type i = 0;i < packages.size();i++)
    {
      assert(!packages[i].stringId.empty());
      assert(revMap.find(packages[i].stringId) == revMap.end());
      revMap.insert(PackageToIdMap::value_type(packages[i].stringId, i));
    }

  ProvideIndexMap provideIndexMap;
  for(PackageVector::size_type i = 0;i < packages.size();i++)
    for(PkgRelVector::size_type j = 0;j < packages[i].provides.size();j++)
    {
      const std::string&^ provideName = packages[i].provides[j].name;
      ProvideIndexMap::iterator it = provideIndexMap.find(provideName);
      if (it == provideIndexMap.end())
	provideIndexMap.insert(ProvideIndexMap::value_type(provideName, ProvideIndex(0, 1))); else 
	it->second.count++;
    }
  size_t count = 0;
  for(ProvideIndexMap::iterator it = provideIndexMap.begin();it != provideIndexMap.end();++it)
    {
      it->pos = count;
      count += it->count;
    }
  PackageIdVector provides;
  provides.resize(count);
  for(PackageIdVector::size_type i = 0;i < count;i++)
    provides[i] = BAD_PACKAGE_ID;
  for(PackageVector::size_type i = 0;i < packages.size();i++)
    for(PkgRelVector::size_type j = 0;j < packages[i].provides.size();j++)
      {
	const std::string& provideName = packages[i].provides[j].name;
	ProvideIndexMap::const_iterator it = provideIndexMap.find(provideName);
	assert(it != provideIndexMap.end());
	assert(it->second.pos + it->second.count <= provides.size());
	size_t k = 0;
	while (k < it->second.count && provides[k + it->second.pos] == BAD_PACKAGE_ID)
	  k++;
	assert(k < it->second.count);
	provides[k + it->second.pos] = i;
      }
}

