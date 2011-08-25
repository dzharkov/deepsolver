

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

typedef std::map<std::string, ProvideIndex> ProvideIndexMap;

static std::auto_ptr<AbstractVersionComparison> versionComparison = createVersionComparison(VersionComparisonRPM);

static void pickPackagesByProvide(const PkgRel& pkgRel, const PackageVector& packages, const PackageIdVector& provides, const ProvideIndexMap& provideIndexMap, PackageIdSet& res)
{
  assert(!pkgRel.name.empty());
  res.clear();
  ProvideIndexMap::const_iterator it = provideIndexMap.find(pkgRel.name);
  if (it == provideIndexMap.end())
	return;//Unmet;
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
	      res.insert(provides[i]);
	      continue;
	    }
	  assert(!pkgRel.version.empty() && !p.version.empty());
	  if (versionComparison->rangesOverlap(pkgRel, PkgRel(p.name, p.getFullVersion(), PkgRel::Equal, 0)))//FIXME:
	    {
	      res.insert(provides[i]);
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
	      res.insert(provides[i]);
	      break;
	    }
	  assert(!pkgRel.version.empty());
	  //rpm-specific behavior: requirement with version specification can be satisfied only by provide with version;
	  if (pp.versionRel == PkgRel::None)
	    continue;
	  assert(!pp.version.empty());
	  if (versionComparison->rangesOverlap(pkgRel, pp))
	    {
	      res.insert(provides[i]);
	      break;
	    }
	} //for(p.provides);
    }//for(packages);
}

static bool       buildSATForPackage(const PackageVector& packages,
				     const PackageIdVector& provides,
				     const ProvideIndexMap& provideIndexMap,
				     PackageId packageId,
				     SAT& sat,
				     const PackageIdSet& done,
				     PackageIdSet& pending)
{
  assert(packageId < packages.size());
  const Package& p = packages[packageId];
  PackageIdSet res;
  //Requires processing;
  for(PkgRelVector::size_type i = 0;i < p.requires.size();i++)
    {
      pickPackagesByProvide(p.requires[i], packages, provides, provideIndexMap, res);
      if (res.empty())
	return 0;//Unmet found;
      Clause clause;
      clause.push_back(Literal(packageId, 1));
      for(PackageIdSet::const_iterator it = res.begin();it != res.end();++it)
	{
	  if (done.find(*it) == done.end())
	    pending.insert(*it);
	  clause.push_back(Literal(*it));
	}
      sat.push_back(clause);
    }//for(requires);
  //Conflicts processing;
  for(PkgRelVector::size_type i = 0;i < p.conflicts.size();i++)
    {
      pickPackagesByProvide(p.conflicts[i], packages, provides, provideIndexMap, res);
      for(PackageIdSet::const_iterator it = res.begin();it != res.end();++it)
	{
	  if (done.find(*it) == done.end())
	    pending.insert(*it);
	  Clause clause;
	  clause.push_back(Literal(packageId, 1));
	  clause.push_back(Literal(*it, 1));
	  sat.push_back(clause);
	}
    }//for(conflicts);
  return 1;
}

bool fillSAT(const PackageVector& packages, PackageId forPackage, SAT& sat)
{
  sat.clear();

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

  assert(forPackage < packages.size());
  const Package& p = packages[forPackage];
  PackageIdSet done, pending;
  PackageIdSet res;
  //Requires processing;
  for(PkgRelVector::size_type i = 0;i < p.requires.size();i++)
    {
      pickPackagesByProvide(p.requires[i], packages, provides, provideIndexMap, res);
      if (res.empty())
	return 0;//Unmet found;
      Clause clause;
      for(PackageIdSet::const_iterator it = res.begin();it != res.end();++it)
	{
	  pending.insert(*it);
	  clause.push_back(Literal(*it));
	}
      sat.push_back(clause);
    }//for(requires);
  //Conflicts processing;
  res.clear();
  for(PkgRelVector::size_type i = 0;i < p.conflicts.size();i++)
    {
      pickPackagesByProvide(p.conflicts[i], packages, provides, provideIndexMap, res);
      for(PackageIdSet::const_iterator it = res.begin();it != res.end();++it)
	{
	  pending.insert(*it);
	  sat.push_back(unitClause(*it, 1));
	}
    }//for(conflicts);

  while(!pending.empty())
    {
      PackageIdSet::iterator it = pending.begin();
      const PackageId packageId = *it;
      done.insert(packageId);
      pending.erase(it);
      if (!buildSATForPackage(packages, provides, provideIndexMap, packageId, sat, done, pending))
	return 0;//Unmet found;
      }

  return 1;
}
