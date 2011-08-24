
#ifndef __APT_NG_SAT_FACTORY_H__
#define __APT_NG_SAT_FACTORY_H__

#include"Package.h"
#include"SATBase.h"

bool fillSAT(const PackageVector& packages, PackageId forPackage, SAT& sat);

#endif //__APT_NG_SAT_FACTORY_H__;
