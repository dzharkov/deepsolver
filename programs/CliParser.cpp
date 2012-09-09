/*
   Copyright 2011-2012 ALT Linux
   Copyright 2011-2012 Michael Pozhidaev

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


#include"deepsolver.h"
#include"CliParser.h"

void CliParser::init(int argc, char* argv[])
{
  assert(argv != NULL);
  if (argc < 1)
    stopNoPrgName();
  m_prgName = argv[0];
  for(int i = 1;i < argc;i++)
    m_params.push_back(argv[i]);
}

void CliParser::parse()
{
  StringVector params;
  for(ParamVector::size_type i = 0;i < m_parasm.size();i++)
    params.push_back(m_params[i].value);
  StringVector::size_type pos = 0;
  size_t clusterValue = 0;
  while (pos < params.size())
    {
      const size_t num = recognizeCluster(params);
      //FIXME:
    }
}

size_t CliParser::recognizeCluster(const StringVector& params) const
{
  //FIXME:
}

void CliParser::stopNoPrgName() const;
