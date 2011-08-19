
#ifndef __APT_NG_SAT_FACTORY_H__
#define __APT_NG_SAT_FACTORY_H__

#include"Package.h"
#include"SATBase.h"

typedef size_t PackageId;
typedef std::vector<PackageId> PackageIdVector;
#define BAD_PACKAGE_ID (PackageId)-1

void fillSAT(const PackageVector& packages, PackageId forPackage, SAT& sat);

#endif //__APT_NG_SAT_FACTORY_H__;
