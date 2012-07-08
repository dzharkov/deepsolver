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

#ifndef DEEPSOLVER_REPO_INDEX_INFO_FILE_H
#define DEEPSOLVER_REPO_INDEX_INFO_FILE_H

class RepoIndexInfoFile
{
public:
  RepoIndexInfoFile() {}

public:
  bool read(const std::string& fileName, std::string& errorMessage, StringList& warningMessages);
  bool write(const std::string& fileName, std::string& errorMessage, StringList& warningMessages);

  std::string getCompressionType() const 
  {
    return m_compressionType;
  }

  void setCompressionType(const std::string compressionType)
  {
    m_compressionType = compressionType;
  }

  std::string getFormatType() const
  {
    return m_formatType;
  }

  void setFormatType(const std::string& formatType)
  {
    m_formatType = formatType;
  }

  std::string getFormatVersion() const
  {
    return m_formatVersion;
  }

  void setFormatVersion(const std::string& formatVersion)
  {
    m_formatVersion = formatVersion;
  }

  std::string getMd5sumFile() const
  {
    return m_md5sumFile;
  }

  void setMd5sumFile(const std::string& md5sumFile)
  {
    m_md5sumFile = md5sumFile;
  }

  void addUserParam(const std::string& name, const std::string& value)
  {
    m_userParams.insert(StringToStringMap::value_type(name, value));
  }

  const StringToStringMap& getUserParams() const
  {
    return m_userParams;
  }

private:
  std::string m_compressionType;
  std::string m_formatType;
  std::string m_formatVersion;
  std::string m_md5sumFile;
  StringToStringMap m_userParams;
}; //class RepoIndexInfoFile;

#endif //DEEPSOLVER_REPO_INDEX_INFO_FILE_H;
