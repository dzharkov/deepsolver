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

#include"deepsolver.h"
#include"version.h"

void runTest(const VersionCond& cond1,
	     const VersionCond& cond2,
	     const std::string& str,
bool correctRes)
{
  const bool res = versionOverlap(cond1, cond2);
  std::cout << "Checking " << str << ": " << (res?"Yes":"No") << " (" << ((res == correctRes)?"OK":"Failed") << ")" << std::endl;
  if (res != correctRes)
    exit(EXIT_FAILURE);
}

int main()
{
  runTest(VersionCond("1.0"), VersionCond("1.0"), "1.0 = 1.0-", 1);
  runTest(VersionCond("1.0-alt1"), VersionCond("1.0-alt1"), "1.0-alt1 = 1.0-alt1", 1);
  runTest(VersionCond("1.0-alt1"), VersionCond("1.0-alt2"), "1.0-alt1 = 1.0-alt2", 0);
  runTest(VersionCond("1.0-alt1"), VersionCond("1.1-alt1"), "1.0-alt1 = 1.1-alt1", 0);
  runTest(VersionCond("1.0-alt1"), VersionCond("1.0"), "1.0-alt1 = 1.0", 1);
  runTest(VersionCond("1.0"), VersionCond("1.0-alt1"), "1.0 = 1.0-alt1", 1);

  runTest(VersionCond("1.0-alt1", VerGreater), VersionCond("1.0-alt2"), "1.0-alt1 < 1.0-alt2", 1);
  runTest(VersionCond("1.0-alt1", VerGreater), VersionCond("1.0-alt0.9"), "1.0-alt1 < 1.0-alt0.9", 0);
  runTest(VersionCond("1.0-alt1", VerGreater | VerEquals), VersionCond("1.0-alt1"), "1.0-alt1 <= 1.0-alt1", 1);
  runTest(VersionCond("1.0-alt5"), VersionCond("1.0-alt2", VerGreater), "1.0-alt5 > 1.0-alt2", 1);

  runTest(VersionCond("1.0", VerGreater), VersionCond("1.5", VerLess), "intersecting >1.0 and <1.5", 1);
  runTest(VersionCond("1.0", VerLess), VersionCond("1.5", VerGreater), "intersecting <1.0 and >1.5", 0);
  runTest(VersionCond("1.0", VerGreater), VersionCond("1.5", VerGreater), "intersecting >1.0 and >1.5", 1);

  runTest(VersionCond("1.0"), VersionCond("1:1.0"), "1.0 = 1:1.0", 0);
  runTest(VersionCond("1:1.0"), VersionCond("1.0"), "1:1.0 = 1.0", 1);
  runTest(VersionCond("1:1.5", VerLess), VersionCond("1.0"), "1:1.5 > 1.0", 1);

  runTest(VersionCond("1:1.5", VerGreater), VersionCond("2:1.0"), "1:1.5 < 2:1.0", 1);
  return 0;
}
