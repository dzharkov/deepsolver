

#include<assert.h>
#include<iostream>
#include<string>
#include<map>
#include<memory>
#include"Package.h"
#include"SATBase.h"
#include"SATFactory.h"
#include"VerCmp.h"

struct ProvideIndex
{
  ProvideIndex(): pos(0), count(0) {}
  ProvideIndex(size_t p, size_t c): pos(p), count(c)  {}

  size_t pos, count; 
}; //struct ProvideIndex;

typedef std::map<std::string, PackageId> PackageToIdMap;
typedef std::map<std::string, ProvideIndex> ProvideIndexMap;

static std::auto_ptr<AbstractVersionComparison> versionComparison = std::auto_ptr<AbstractVersionComparison>(new VersionReleaseComparison(createVersionComparison(VersionComparisonRPM)));

static bool verCmp(int acceptable, const std::string& required, const std::string& checking)
{
  const int res = versionComparison->compare(required, checking);
  if (res == 0)//Means equal versions;
    return acceptable == PkgRel::Equal || acceptable == PkgRel::LessOrEqual || acceptable == PkgRel::GreaterOrEqual;
  if (res > 0)//Required greater than checking;
    return acceptable == PkgRel::Less || acceptable == PkgRel::LessOrEqual;
  if (res < 0)//Checking greater than required;
    return acceptable == PkgRel::Greater || acceptable == PkgRel::GreaterOrEqual;
  assert(0);
  return 0;//Just to reduce warning messages;
}

static bool versionIntersection(const PkgRel& p1, const PkgRel& p2)
{
  const int res = versionComparison->compare(p1.version, p2.version);
  if (res == 0)
    return p1.canBeEqual() && p2.canBeEqual();
  PkgRel less, greater;
  if (res < 0)
    {
      less = p1;
      greater = p2;
    } else
    {
      assert(res > 0);
      less = p2;
      greater = p1;
    }
  int lessFirst = 0, first = 0, middle = 0, second = 0, greaterSecond = 0;
  if (less.canBeLess())
    lessFirst++;
  if (less.canBeEqual())
    first++;
  if (less.canBeGreater())
    {
      middle++;
      second++;
      greaterSecond++;
    }
  if (greater.canBeLess())
    {
      lessFirst++;
      first++;
      middle++;
    }
  if (greater.canBeEqual())
    second++;
  if (greater.canBeGreater())
    greaterSecond++;
  return lessFirst == 2 || first == 2 || middle == 2 || second == 2 || greaterSecond == 2;
}

static void pickPackagesByProvide(const PkgRel& pkgRel, const PackageVector& packages, const PackageIdVector& provides, const ProvideIndexMap& provideIndexMap, PackageIdVector& res)
{
  assert(!pkgRel.name.empty());
  res.clear();
  ProvideIndexMap::const_iterator it = provideIndexMap.find(pkgRel.name);
  assert(it != provideIndexMap.end());
  const size_t pos = it->second.pos;
  const size_t count = it->second.count;
  assert(pos + count <= provides.size());
  for(PackageIdVector::size_type i = pos;i < pos + count;i++)
    {
      assert(provides[i] < packages.size());
      const Package& p = packages[provides[i]];
      if (p.name == pkgRel.name)
	{
	  //Checking if the package is appropriate by itself;
	  if (pkgRel.versionRel == PkgRel::None)
	    {
	      //Any version is accessible for this relation;
	      res.push_back(i);
	      continue;
	    }
	  assert(!pkgRel.version.empty() && !p.version.empty());
	  if (verCmp(pkgRel.versionRel, pkgRel.version, p.version))
	    {
	      res.push_back(i);
	      continue;
	    }
	} //if (p.name == pkgRel.name);
      //Checking all provides of the package;
      for(PkgRelVector::size_type j = 0;j < p.provides.size();j++)
	{
	  const PkgRel& pp = p.provides[j];
	  if (pkgRel.name != pp.name)
	    continue;
	  if (pkgRel.versionRel == PkgRel::None)
	    {
	      //We can take any version;
	      res.push_back(i);
	      break;
	    }
	  assert(!pkgRel.version.empty());
	  //rpm-specific behavior: requirement with version specification can be satisfied only by provide with version;
	  if (pp.versionRel != PkgRel::None)
	    continue;
	  assert(!pp.version.empty());
	  if (versionIntersection(pkgRel, pp))
	    {
	      res.push_back(i);
	      break;
	    }
	} //for(p.provides);
    }//for(packages);
}

void fillSAT(const PackageVector& packages, PackageId forPackage, SAT& sat)
{
  PackageToIdMap revMap;
  for(PackageVector::size_type i = 0;i < packages.size();i++)
    {
      assert(!packages[i].stringId.empty());
      assert(revMap.find(packages[i].stringId) == revMap.end());
      revMap.insert(PackageToIdMap::value_type(packages[i].stringId, i));
    }

  ProvideIndexMap provideIndexMap;
  //We do not rely on rpm feature to force each package to provide itself;
  for(PackageVector::size_type i = 0;i < packages.size();i++)
    {
      const std::string& name = packages[i].name;
      ProvideIndexMap::iterator it = provideIndexMap.find(name);
      if (it == provideIndexMap.end())
	provideIndexMap.insert(ProvideIndexMap::value_type(name, ProvideIndex(0, 1))); else 
	it->second.count++;
    }
  for(PackageVector::size_type i = 0;i < packages.size();i++)
    for(PkgRelVector::size_type j = 0;j < packages[i].provides.size();j++)
      {
	const std::string& provideName = packages[i].provides[j].name;
	ProvideIndexMap::iterator it = provideIndexMap.find(provideName);
	if (it == provideIndexMap.end())
	  provideIndexMap.insert(ProvideIndexMap::value_type(provideName, ProvideIndex(0, 1))); else 
	  it->second.count++;
      }

  size_t count = 0;
  PackageIdVector provides;
  for(ProvideIndexMap::iterator it = provideIndexMap.begin();it != provideIndexMap.end();++it)
    {
      assert(it->second.count > 0);
      it->second.pos = count;
      count += it->second.count;
    }
  provides.resize(count);
  for(PackageIdVector::size_type i = 0;i < count;i++)
    provides[i] = BAD_PACKAGE_ID;

  for(PackageVector::size_type i = 0;i < packages.size();i++)
    {
      const std::string& name = packages[i].name;
      ProvideIndexMap::const_iterator it = provideIndexMap.find(name);
      assert(it != provideIndexMap.end());
      assert(it->second.pos + it->second.count <= provides.size());
      size_t k = 0;
      while (k < it->second.count && provides[k + it->second.pos] != BAD_PACKAGE_ID)
	k++;
      assert(k < it->second.count);
      provides[k + it->second.pos] = i;
    }
  for(PackageVector::size_type i = 0;i < packages.size();i++)
    for(PkgRelVector::size_type j = 0;j < packages[i].provides.size();j++)
      {
	const std::string& provideName = packages[i].provides[j].name;
	ProvideIndexMap::const_iterator it = provideIndexMap.find(provideName);
	assert(it != provideIndexMap.end());
	assert(it->second.pos + it->second.count <= provides.size());
	size_t k = 0;
	while (k < it->second.count && provides[k + it->second.pos] != BAD_PACKAGE_ID)
	  k++;
	assert(k < it->second.count);
	provides[k + it->second.pos] = i;
      }

  //End of preparing;
}

