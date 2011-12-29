
#include"depsolver.h"
#include"SatConstructor.h"

void SatConstructor::construct(const VarIdVector& strongToInstall,
			       const VarIdVector& strongToRemove,
			       Sat& sat) const
{
  m_pendingToInstall.clear();
  m_pendingToRemove.clear();
  for(VarIdVector::size_type i = 0;i < strongToInstall.size();i++)
    m_pendingToInstall.insert(strongToInstall[i]);

  for(VarIdVector::size_type i = 0;i < strongToRemove.ssize();i++)
    m_pendingToInstall.insert(strongToInstall[i]);


}
