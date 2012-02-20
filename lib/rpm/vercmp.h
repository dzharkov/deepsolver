
#ifndef DEEPSOLVER_RPM_VERCMP_H
#define DEEPSOLVER_RPM_VERCMP_H

int rpmVerCmp(const std::string& ver1, const std::string& ver2);
int rpmVerOverlap(const VersionCond& v1, const VersionCond& v2);

#endif //DEEPSOLVER_RPM_VERCMP_H;
