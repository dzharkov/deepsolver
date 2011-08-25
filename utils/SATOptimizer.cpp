
#include"SATOptimizer.h"

void optimizeSAT(SAT& sat)
{
  //Not effective implementation, need futher research;
  SAT::iterator it1 = sat.begin();
  while (it1 != sat.end())
    {
      SAT::iterator it2 = sat.begin();
      while(it2 != it1 && !equalClauses(*it1, *it2))
	it2++;
      if (it2 == it1)
	{
	  it1++;
	continue;
	}
      it2 = it1;
      it1++;
      sat.erase(it2);
    }
}
