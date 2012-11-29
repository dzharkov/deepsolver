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
#include"TransactionProgress.h"

void TransactionProgress::onAvailablePkgListProcessing()
{
  if (m_suppress)
    return;
  m_stream << "Processing package lists from all repositories" << std::endl;
}

void TransactionProgress::onInstalledPkgListProcessing()
{
  if (m_suppress)
    return;
  m_stream << "Processing installed package list" << std::endl;
}

void TransactionProgress::onInstallRemovePkgListProcessing()
{
  if (m_suppress)
    return;
  m_stream << "Calculating package lists for installation and removing" << std::endl;
}
