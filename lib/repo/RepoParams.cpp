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
#include"InfoFileWriter.h"

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

void RepoIndexInfoFile::write(const std::string& fileName, const StringToStringMap& params) const
{
  std::ostringstream ss;
  time_t t;
  time(&t);
  ss << "# Repository index information file" << std::endl;
  ss << "# Generated on " << ctime(&t) << std::endl;
  ss << "# This file contains a set of options to control various parameters of" << std::endl;
  ss << "# repository index structure. An empty lines are silently ignored. Any" << std::endl;
  ss << "# line text after the character `#\' is skipped as a comment. " << std::endl;
  ss << "# Character `\\\' should be used in the conjunction with the following character to" << std::endl;
  ss << "# prevent special character processing." << std::endl;
  ss << std::endl;
  for(StringToStringMap::const_iterator it = params.begin();it != params.end();it++)
      ss << it->first << " = " << escapeString(it->second) << std::endl;
  File f;
  f.create(fileName);
  f.write(ss.str().c_str(), ss.str().length());
}

void IndexCore::writeInfoFile(const std::string& fileName, const RepoIndexParams& params)
{
  RepoIndexInfoFile infoFile;
  switch(params.compressionType)
    {
    case RepoIndexParams::CompressionTypeNone:
      infoFile.setCompressionType("none");
      break;
    case RepoIndexParams::CompressionTypeGzip:
      infoFile.setCompressionType("gzip");
      break;
    default:
      assert(0);
    }; //switch(compressionType);
  switch(params.formatType)
    {
    case RepoIndexParams::FormatTypeText:
      infoFile.setFormatType("text");
      break;
    case RepoIndexParams::FormatTypeBinary:
      infoFile.setFormatType("binary");
      break;
    default:
      assert(0);
    }; //switch(formatType);
  infoFile.setFormatVersion(PACKAGE_VERSION);
  infoFile.setMd5sumFile(REPO_INDEX_MD5SUM_FILE);
  for(StringToStringMap::const_iterator it = params.userParams.begin();it != params.userParams.end();it++)
    infoFile.addUserParam(it->first, it->second);
  std::string errorMessage;
  StringList warningMessages;
  const bool res = infoFile.write(fileName, errorMessage, warningMessages);
  for(StringList::const_iterator it = warningMessages.begin();it != warningMessages.end();it++)
    m_warningHandler.onWarning(*it);
  if (!res)
    INDEX_CORE_STOP(errorMessage);
}

