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
#include"OperationCore.h"
#include"Repository.h"
#include"utils/TinyFileDownload.h"
#include"repo/InfoFileReader.h"
#include"repo/TextFormatReader.h"

void Repository::fetchInfoAndChecksum()
{
  const std::string infoFileUrl = buildInfoFileUrl();
  logMsg(LOG_DEBUG, "Fetching tiny file \'%s\'", infoFileUrl.c_str());
  TinyFileDownload download;//FIXME:max file size limit;
  download.fetch(infoFileUrl);
  const std::string info = download.getContent();
  InfoFileReader reader;
  StringToStringMap infoValues;
  try {
    reader.read(info, infoValues);
  }
  catch(const InfoFileException& e)
    {
      logMsg(LOG_ERR, "info file parsing problem:%s", e.getMessage().c_str());
      throw OperationException(OperationErrorInvalidInfoFile);
    }
  logMsg(LOG_DEBUG, "Info file downloaded and parsed, list of values:");
  for(StringToStringMap::const_iterator it = infoValues.begin();it != infoValues.end();it++)
    logMsg(LOG_DEBUG, "info file value: \'%s\' = \'%s\'", it->first.c_str(), it->second.c_str());
  if (infoValues.find(INFO_FILE_FORMAT_TYPE) == infoValues.end())
    {
      logMsg(LOG_ERR, "Info file does not contain the \'%s\' key", INFO_FILE_FORMAT_TYPE);
      throw OperationException(OperationErrorInvalidInfoFile);
    }
  if (infoValues.find(INFO_FILE_COMPRESSION_TYPE) == infoValues.end())
    {
      logMsg(LOG_ERR, "Info file does not contain the \'%s\' key", INFO_FILE_COMPRESSION_TYPE);
      throw OperationException(OperationErrorInvalidInfoFile);
    }
  if (infoValues.find(INFO_FILE_MD5SUM) == infoValues.end())
    {
      logMsg(LOG_ERR, "Info file does not contain the \'%s\' key", INFO_FILE_MD5SUM);
      throw OperationException(OperationErrorInvalidInfoFile);
    }
  const std::string formatType = trim(infoValues.find(INFO_FILE_FORMAT_TYPE)->second);
  const std::string compressionType = trim(infoValues.find(INFO_FILE_COMPRESSION_TYPE)->second);
  m_checksumFileName = trim(infoValues.find(INFO_FILE_MD5SUM)->second);
  if (formatType == INFO_FILE_FORMAT_TYPE_TEXT)
    m_formatType = FormatTypeText; else
    if (formatType == INFO_FILE_FORMAT_TYPE_BINARY)
      m_formatType = FormatTypeBinary; else
      {
	logMsg(LOG_ERR, "Unsupported format type in info file: \'%s\'", formatType.c_str());
	throw OperationException(OperationErrorInvalidInfoFile);
      }
  if (compressionType == INFO_FILE_COMPRESSION_TYPE_NONE)
    m_compressionType = CompressionTypeNone; else
    if (compressionType == INFO_FILE_COMPRESSION_TYPE_GZIP)
      m_compressionType = CompressionTypeGzip; else
      {
	logMsg(LOG_ERR, "Unsupported compression type in info file: \'%s\'", compressionType.c_str());
	throw OperationException(OperationErrorInvalidInfoFile);
      }
}

void Repository::addIndexFilesForFetch(StringToStringMap& files)
{
  assert(m_formatType == FormatTypeText || m_formatType == FormatTypeBinary);
  assert(m_compressionType == CompressionTypeNone || m_compressionType == CompressionTypeGzip);
  assert(!m_url.empty());
  assert(!m_arch.empty());
  assert(!m_component.empty());
  std::string dir = m_url;
  if (dir[dir.size() - 1] != '/')
    dir += '/';
  dir += m_arch + "/";
  dir += REPO_INDEX_DIR_PREFIX + m_component + "/";
  m_mainPkgFileUrl = dir + REPO_INDEX_PACKAGES_DATA_FILE;
  m_providesFileUrl = dir + REPO_INDEX_PROVIDES_DATA_FILE;
  if (m_compressionType == CompressionTypeGzip)
    {
      m_mainPkgFileUrl += COMPRESSION_SUFFIX_GZIP;
      m_providesFileUrl += COMPRESSION_SUFFIX_GZIP;
    }
  files.insert(StringToStringMap::value_type(m_mainPkgFileUrl, ""));
  files.insert(StringToStringMap::value_type(m_providesFileUrl, ""));
  //Here may be also sources index file but question about it must be considered later;
}

void Repository::addPackagesToScope(const StringToStringMap& files, PackageScopeContentBuilder& content)
{
  int textFileType;
  if (m_compressionType == CompressionTypeNone)
    textFileType = TextFileStd; else 
    if (m_compressionType == CompressionTypeGzip)
      textFileType = TextFileGZip; else
      {
	assert(0);
      }
  StringToStringMap::const_iterator it = files.find(m_mainPkgFileUrl);
  assert(it != files.end());
  const std::string pkgFileName = it->second;
  it = files.find(m_providesFileUrl);
  assert (it != files.end());
  const std::string providesFileName = it->second;
  //FIXME:checksum processing;
  TextFormatReader reader;
  logMsg(LOG_DEBUG, "Reading packages information from \'%s\' for repository \'%s\'", pkgFileName.c_str(), m_name.c_str());
  try {
    reader.openFile(pkgFileName, textFileType);
    PkgFile pkgFile;
    while(reader.readPackage(pkgFile))
      content.addPkg(pkgFile);
  }
  catch(const TextFormatReaderException& e)
    {
      logMsg(LOG_ERR, "Repo \'%s\':package data in text format error:%s", m_name.c_str(), e.getMessage().c_str());
      throw OperationException(OperationErrorBrokenIndexFile);
    }
  reader.close();
  logMsg(LOG_DEBUG, "Package data from \'%s\' was loaded successfully", pkgFileName.c_str());
  logMsg(LOG_DEBUG, "Reading provides information from \'%s\' for repository \'%s\'", providesFileName.c_str(), m_name.c_str());
  try {
    reader.openFile(providesFileName, textFileType);
    std::string provideName;
    StringVector providers;
    while(reader.readProvides(provideName, providers))
      {
	assert(!provideName.empty());
	for(StringVector::size_type i = 0;i < providers.size();i++ )
	  content.addProvideMapItem(provideName, providers[i]);
      }
  }
  catch (const TextFormatReaderException& e)
    {
      logMsg(LOG_ERR, "Repo \'%s\':provides data in text format error:%s", m_name.c_str(), e.getMessage().c_str());
      throw OperationException(OperationErrorBrokenIndexFile);
    }
  reader.close();
}

std::string Repository::buildInfoFileUrl() const
{
  assert(!m_url.empty());
  assert(!m_arch.empty());
  assert(!m_component.empty());
  std::string value = m_url;
  if (value[value.size() - 1] != '/')
    value += '/';
  value += m_arch + "/";
  value += REPO_INDEX_DIR_PREFIX + m_component + "/";
  value += REPO_INDEX_INFO_FILE;
  return value;
}
