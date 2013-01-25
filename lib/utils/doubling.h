/*
   Copyright 2011-2013 ALT Linux
   Copyright 2011-2013 Michael Pozhidaev

   This file is part of the Deepsolver.

   Deepsolver is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   Deepsolver is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
*/


#ifndef DEEPSOLVER_DOUBLING_H
#define DEEPSOLVER_DOUBLING_H

template<typename T>
void rmDub(std::vector<T>& v)
{
  if (v.size() < 64)
    {
      std::vector<T> vv;
      for (size_t i = 0;i < v.size();i++)
	{
	  size_t j;
	  for(j = 0;j < i;j++)
	    if (v[i] == v[j])
	      break;
	  if (j == i)
	  vv.push_back(v[i]);
	}
      v = vv;
      return;
    }
  //Be careful: the following only for items with operator < and operator >;
  std::set<T> s;
  for(size_t i = 0;i < v.size();i++)
    s.insert(v[i]);
  v.resize(s.size());
  size_t k = 0;
  typename std::set<T>::const_iterator it;
  for(it = s.begin();it != s.end();it++)
    v[k++] = *it;
  assert(k == s.size());
}

#endif //DEEPSOLVER_DOUBLING_H;

