
#ifndef FIXME_PKG_H
#define FIXME_PKG_H

#include"DateTime.h"

typedef unsigned short Epoch;

class Pkg
{
public:
  Pkg():
    epoch(0) {}
  virtual ~Pkg() {}

protected:
  Epoch m_epoch;
  std::string m_name;
  std::string m_version;
  std::string m_release;
  std::string m_arch;
  std::string m_packager;
  std::string m_url;
  std::string m_license;
  std::string m_summary;
  std::string m_descr;
  DateTime m_buildDateTime;
}; //class Pkg;

#endif //FIXME_PKG_H;
