/*
   Copyright 2011-2012 ALT Linux
   Copyright 2011-2012 Dmitry V. Levin
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
#include"RepoIndexInfoFile.h"

static std::string escapeString(const std::string& s)
{
  std::string res;
  for(std::string::size_type i = 0;i < s.length();i++)
    {
      switch(s[i])
	{
	case '\\':
	  res += "\\\\";
	  break;
	case '#':
	  res += "\\#";
	  break;
	default:
	  res += s[i];
	}; //switch();
    }
  return res;
}

static void writeParam(std::ostream& s, const std::string& name, const std::string& value)
{
  s << name << " = " << escapeString(value) << std::endl;
}

bool RepoIndexInfoFile::read(const std::string& fileName, std::string& errorMessage, StringList& warningMessages)
{
  errorMessage.erase();
  warningMessages.clear();
  return 0;
}

bool RepoIndexInfoFile::write(const std::string& fileName, std::string& errorMessage, StringList& warningMessages)
{
  errorMessage.erase();
  warningMessages.clear();
  std::ofstream f(fileName.c_str());
  if (!f)
    {
      errorMessage = "Could not create \'" + fileName + "\' for writing";
      return 0;
    }
  time_t t;
  time(&t);
  f << "# Repository index information file" << std::endl;
    f << "# Generated on " << ctime(&t) << std::endl;
  f << "# This file contains a set of options to control various parameters of" << std::endl;
  f << "# repository index structure. An empty lines are silently ignored. Any" << std::endl;
  f << "# line text after the character `#\' is skipped as a comment. " << std::endl;
  f << "# Character `\\\' should be used in the conjunction with the following character to" << std::endl;
  f << "# prevent special character processing." << std::endl;
  f << std::endl;
  writeParam(f, "format_version", m_formatVersion);
  writeParam(f, "format_type", m_formatType);
  writeParam(f, "compression_type", m_compressionType);
  writeParam(f, "md5sum_file", m_md5sumFile);
  for(StringToStringMap::const_iterator it = m_userParams.begin();it != m_userParams.end();it++)
    writeParam(f, it->first, it->second);
  return 1;
}
