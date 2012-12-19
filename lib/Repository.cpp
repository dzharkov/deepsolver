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
#include"repo/TextFormatSectionReader.h"
#include"repo/PkgSection.h"

static std::auto_ptr<AbstractTextFormatSectionReader> createReader(const std::string& fileName, char compressionType);
static void splitSectionLines(const std::string& sect, StringVector& lines);

void Repository::fetchInfoAndChecksum()
{
  const std::string infoFileUrl = buildInfoFileUrl();
  logMsg(LOG_DEBUG, "Constructed info file URL is \'%s\'", infoFileUrl.c_str());
  TinyFileDownload download;//FIXME:max file size limit;
  download.fetch(infoFileUrl);
  const std::string infoFileContent = download.getContent();
  InfoFileReader reader;
  StringToStringMap infoValues;
  try {
    reader.read(infoFileContent, infoValues);
  }
  catch(const InfoFileException& e)
    {
      logMsg(LOG_ERR, "info file parsing problem:%s", e.getMessage().c_str());
      throw OperationException(OperationException::InvalidInfoFile, infoFileUrl);
    }
  logMsg(LOG_DEBUG, "Info file downloaded and parsed, list of values:");
  for(StringToStringMap::const_iterator it = infoValues.begin();it != infoValues.end();it++)
    logMsg(LOG_DEBUG, "info file value: \'%s\' = \'%s\'", it->first.c_str(), it->second.c_str());
  if (infoValues.find(INFO_FILE_FORMAT_TYPE) == infoValues.end())
    {
      logMsg(LOG_ERR, "Info file does not contain the \'%s\' key", INFO_FILE_FORMAT_TYPE);
      throw OperationException(OperationException::InvalidInfoFile, infoFileUrl);
    }
  if (infoValues.find(INFO_FILE_MD5SUM) == infoValues.end())
    {
      logMsg(LOG_ERR, "Info file does not contain the \'%s\' key", INFO_FILE_MD5SUM);
      throw OperationException(OperationException::InvalidInfoFile, infoFileUrl);
    }
  m_checksumFileName = trim(infoValues.find(INFO_FILE_MD5SUM)->second);
  const std::string checksumFileUrl = buildChecksumFileUrl();
  download.fetch(checksumFileUrl);
  Md5File md5File;
  try {
    md5File.loadFromString(download.getContent(), checksumFileUrl);
  }
  catch(const Md5FileException& e)
    {
      logMsg(LOG_ERR, "Checksum file problem:%s", e.getMessage().c_str());
      throw OperationException(OperationException::InvalidChecksumData, checksumFileUrl);
    }
  Md5File::ItemVector::size_type i;
  for(i = 0;i < md5File.items.size();i++)
    if (md5file.items[i].fileName == REPO_INDEX_INFO_FILE)
      break;
  if (i >= md5File.items.size())
    {
      logMsg(LOG_ERR, "Checksum file from \'%\' has no entry for info file (\'%s\')", checksumFileUrl.c_str(), REPO_INDEX_INFO_FILE);
      throw OperationException(OperationException::InvalidChecksumData, checksumFileUrl);
    }
  if (!md5File.verifyItem(i, Directory::mixNameComponents(params.indexPath, md5File.items[i].fileName)))



  if (infoValues.find(INFO_FILE_COMPRESSION_TYPE) == infoValues.end())
    {
      logMsg(LOG_ERR, "Info file does not contain the \'%s\' key", INFO_FILE_COMPRESSION_TYPE);
      throw OperationException(OperationErrorInvalidInfoFile);
    }
  const std::string formatType = trim(infoValues.find(INFO_FILE_FORMAT_TYPE)->second);
  const std::string compressionType = trim(infoValues.find(INFO_FILE_COMPRESSION_TYPE)->second);
  logMsg(LOG_DEBUG, "Checksum file name is \'%s\'", m_checksumFileName.c_str());
  logMsg(LOG_DEBUG, "Format type is \'%s\'", formatType.c_str());
  logMsg(LOG_DEBUG, "Compression type is \'%s\'", compressionType.c_str());
  if (formatType == INFO_FILE_FORMAT_TYPE_TEXT)
    m_formatType = RepoParams::FormatTypeText; else
    if (formatType == INFO_FILE_FORMAT_TYPE_BINARY)
      m_formatType = RepoParams::FormatTypeBinary; else
      {
	logMsg(LOG_ERR, "Unsupported format type in info file: \'%s\'", formatType.c_str());
	throw OperationException(OperationErrorInvalidInfoFile);
      }
  if (compressionType == INFO_FILE_COMPRESSION_TYPE_NONE)
    m_compressionType = RepoParams::CompressionTypeNone; else
    if (compressionType == INFO_FILE_COMPRESSION_TYPE_GZIP)
      m_compressionType = RepoParams::CompressionTypeGzip; else
      {
	logMsg(LOG_ERR, "Unsupported compression type in info file: \'%s\'", compressionType.c_str());
	throw OperationException(OperationErrorInvalidInfoFile);
      }
}

void Repository::addIndexFilesForFetch(StringToStringMap& files)
{
  assert(m_formatType == RepoParams::FormatTypeText || m_formatType == RepoParams::FormatTypeBinary);
  assert(m_compressionType == RepoParams::CompressionTypeNone || m_compressionType == RepoParams::CompressionTypeGzip);
  assert(!m_url.empty());
  assert(!m_arch.empty());
  assert(!m_component.empty());
  std::string dir = m_url;
  if (dir[dir.size() - 1] != '/')
    dir += '/';
  dir += m_arch + "/";
  dir += REPO_INDEX_DIR_PREFIX + m_component + "/";
  logMsg(LOG_DEBUG, "Constructing list of files to download, basic URL is \'%s\'", dir.c_str());
  m_pkgFileUrl = dir + REPO_INDEX_PACKAGES_FILE;
  m_pkgDescrFileUrl = dir + REPO_INDEX_PACKAGES_DESCR_FILE;
m_srcFileUrl = dir + REPO_INDEX_SOURCES_FILE;
m_srcDescrFileUrl = dir + REPO_INDEX_SOURCES_DESCR_FILE;
 if (m_compressionType == RepoParams::CompressionTypeGzip)
    {
      m_pkgFileUrl += COMPRESSION_SUFFIX_GZIP;
      m_pkgDescrFileUrl += COMPRESSION_SUFFIX_GZIP;
      m_srcFileUrl += COMPRESSION_SUFFIX_GZIP;
      m_srcDescrFileUrl += COMPRESSION_SUFFIX_GZIP;
    }
  files.insert(StringToStringMap::value_type(m_pkgFileUrl, ""));
  files.insert(StringToStringMap::value_type(m_pkgDescrFileUrl, ""));
  files.insert(StringToStringMap::value_type(m_srcFileUrl, ""));
  files.insert(StringToStringMap::value_type(m_srcDescrFileUrl, ""));
}

void Repository::loadPackageData(const StringToStringMap& files, 
		     AbstractPackageRecipient& transactData,
		     AbstractPackageRecipient& pkgInfoData)
{
  StringToStringMap::const_iterator it = files.find(m_pkgFileUrl);
  assert(it != files.end());
  const std::string pkgFileName = it->second;
  it = files.find(m_pkgDescrFileUrl);
  assert(it != files.end());
  const std::string pkgDescrFileName = it->second;
  it = files.find(m_srcFileUrl);
  assert(it != files.end());
  const std::string srcFileName = it->second;
  it = files.find(m_srcDescrFileUrl);
  assert(it != files.end());
  const std::string srcDescrFileName = it->second;
  logMsg(LOG_DEBUG, "pkgFileName = \'%s\'", pkgFileName.c_str());
  logMsg(LOG_DEBUG, "pkgDescrFileName = \'%s\'", pkgDescrFileName.c_str());
  logMsg(LOG_DEBUG, "srcFileName = \'%s\'", srcFileName.c_str());
  logMsg(LOG_DEBUG, "srcDescrFileName = \'%s\'", srcDescrFileName.c_str());
  size_t invalidLineNum;
  std::string invalidLineValue;
  std::string sect;
  StringVector lines;
  logMsg(LOG_DEBUG, "Reading transact data");
  std::auto_ptr<AbstractTextFormatSectionReader> reader = createReader(pkgFileName, m_compressionType);
  reader->init();
  while(reader->readNext(sect))
    {
      PkgFile pkgFile;
      splitSectionLines(sect, lines);
      if (!PkgSection::parsePkgFileSection(lines, pkgFile, invalidLineNum, invalidLineValue))
	{
	  logMsg(LOG_ERR, "Broken index file \'%s\', invalid line %zu in section \'%s\': \'%s\'", m_pkgFileUrl.c_str(), invalidLineNum, pkgFile.fileName.c_str(), invalidLineValue.c_str());
	  throw OperationException(OperationErrorBrokenIndexFile);
	}
      pkgFile.isSource = 0;
      transactData.onNewPkgFile(pkgFile);
    }
  reader->close();
  logMsg(LOG_DEBUG, "Reading binary packages descriptions");
  reader = createReader(pkgDescrFileName, m_compressionType);
  reader->init();
  while(reader->readNext(sect))
    {
      PkgFile pkgFile;
      splitSectionLines(sect, lines);
      if (!PkgSection::parsePkgFileSection(lines, pkgFile, invalidLineNum, invalidLineValue))
	{
	  logMsg(LOG_ERR, "Broken index file \'%s\', invalid line %zu in section \'%s\': \'%s\'", m_pkgDescrFileUrl.c_str(), invalidLineNum, pkgFile.fileName.c_str(), invalidLineValue.c_str());
	  throw OperationException(OperationErrorBrokenIndexFile);
	}
      pkgFile.isSource = 0;
      pkgInfoData.onNewPkgFile(pkgFile);
    }
  reader->close();
  logMsg(LOG_DEBUG, "Reading source packages list");
  reader = createReader(srcFileName, m_compressionType);
  reader->init();
  while(reader->readNext(sect))
    {
      PkgFile pkgFile;
      splitSectionLines(sect, lines);
      if (!PkgSection::parsePkgFileSection(lines, pkgFile, invalidLineNum, invalidLineValue))
	{
	  logMsg(LOG_ERR, "Broken index file \'%s\', invalid line %zu in section \'%s\': \'%s\'", m_srcFileUrl.c_str(), invalidLineNum, pkgFile.fileName.c_str(), invalidLineValue.c_str());
	  throw OperationException(OperationErrorBrokenIndexFile);
	}
      pkgFile.isSource = 1;
      pkgInfoData.onNewPkgFile(pkgFile);
    }
  reader->close();
  logMsg(LOG_DEBUG, "Reading source packages descriptions");
  reader = createReader(srcDescrFileName, m_compressionType);
  reader->init();
  while(reader->readNext(sect))
    {
      PkgFile pkgFile;
      splitSectionLines(sect, lines);
      if (!PkgSection::parsePkgFileSection(lines, pkgFile, invalidLineNum, invalidLineValue))
	{
	  logMsg(LOG_ERR, "Broken index file \'%s\', invalid line %zu in section \'%s\': \'%s\'", m_srcDescrFileUrl.c_str(), invalidLineNum, pkgFile.fileName.c_str(), invalidLineValue.c_str());
	  throw OperationException(OperationErrorBrokenIndexFile);
	}
      pkgFile.isSource = 1;
      pkgInfoData.onNewPkgFile(pkgFile);
    }
  reader->close();
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

std::auto_ptr<AbstractTextFormatSectionReader> createReader(const std::string& fileName, char compressionType)
{
  if (compressionType == RepoParams::CompressionTypeNone)
    {
      std::auto_ptr<TextFormatSectionReader> reader(new TextFormatSectionReader());
      reader->open(fileName);
      return std::auto_ptr<AbstractTextFormatSectionReader>(reader.release());
    }
  if (compressionType == RepoParams::CompressionTypeGzip)
    {
      std::auto_ptr<TextFormatSectionReaderGzip> reader(new TextFormatSectionReaderGzip());
      reader->open(fileName);
      return std::auto_ptr<AbstractTextFormatSectionReader>(reader.release());
    }
  assert(0);
  return std::auto_ptr<AbstractTextFormatSectionReader>();
}

void splitSectionLines(const std::string& sect, StringVector& lines)
{
  lines.clear();
  std::string line;
  for(std::string::size_type i = 0;i < sect.length();i++)
    {
      if (sect[i] == '\r')
	continue;
      if (sect[i] == '\n')
	{
	  line = trim(line);
	  if (!line.empty())
	    lines.push_back(line);
	  line.erase();
	  continue;
	}
      line += sect[i];
    }
  line = trim(line);
  if (!line.empty())
    lines.push_back(line);
}
