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
#include"TextFormatWriter.h"
#include"IndexCoreException.h"

#define TMP_FILE "tmp_packages_data1"

#define NAME_STR "n="
#define EPOCH_STR "e="
#define VERSION_STR "v="
#define RELEASE_STR "r="
#define ARCH_STR "arch="
#define URL_STR "URL="
#define LICENSE_STR "lic="
#define PACKAGER_STR "pckgr="
#define SUMMARY_STr "summ="
#define DESCRIPTION_STR "descr="
#define SRCRPM_STR "src="
#define BUILDTIME_STR "btime="

#define PROVIDES_STR "p:"
#define REQUIRES_STR "r:"
#define CONFLICTS_STR "c:"
#define OBSOLETES_STR "o:"
#define CHANGELOG_STR "cl:"

TextFormatWriter::TextFormatWriter(AbstractIndexConstructionListener& listener,
				   const AbstractRequireFilter& requireFilter,
				   const RepoIndexParams& params,
				   const std::string& dir,
				   const StringSet& additionalRefs)
  : m_listener(listener),
    m_requireFilter(requireFilter),
    m_params(params),
    m_dir(dir),
    m_packagesFileName(addCompressionExtension(Directory::mixNameComponents(dir, REPO_INDEX_PACKAGES_DATA_FILE), params)),
    m_sourcesFileName(addCompressionExtension(Directory::mixNameComponents(dir, REPO_INDEX_SOURCE_DATA_FILE), params)),
    m_tmpFileName(Directory::mixNameComponents(dir, TMP_FILE)),
    m_filterProvidesByRefs(params.provideFilteringByRefs),
    m_additionalRefs(additionalRefs),
    m_filterProvidesByDirs(params.provideFilterDirs)
{
}

void TextFormatWriter::initBinary()
{
  m_tmpFile = createTextFileWriter(TextFileStd, m_tmpFileName);
}

void TextFormatWriter::initSource()
{
  m_sourcesFile = createTextFileWriter(mapTextFileType(m_params.compressionType), m_sourcesFileName);
}

void TextFormatWriter::addBinary(const PkgFile& pkgFile, const StringList& fileList)
{
  m_tmpFile->writeLine("[" + File::baseName(pkgFile.fileName) + "]");
  m_tmpFile->writeLine(NAME_STR + pkgFile.name);
  std::ostringstream epochStr;
  epochStr << EPOCH_STR << pkgFile.epoch;
  m_tmpFile->writeLine(epochStr.str());
  m_tmpFile->writeLine(VERSION_STR + pkgFile.version);
  m_tmpFile->writeLine(RELEASE_STR + pkgFile.release);
  m_tmpFile->writeLine(ARCH_STR + pkgFile.arch);
  m_tmpFile->writeLine(URL_STR + pkgFile.url);
  m_tmpFile->writeLine(LICENSE_STR + pkgFile.license);
  m_tmpFile->writeLine(PACKAGER_STR + pkgFile.packager);
  m_tmpFile->writeLine(SUMMARY_STr + pkgFile.summary);
  m_tmpFile->writeLine(DESCRIPTION_STR + encodeMultiline(pkgFile.description));
  m_tmpFile->writeLine(SRCRPM_STR + pkgFile.srcRpm);
  std::ostringstream bt;
  bt << pkgFile.buildTime;
  m_tmpFile->writeLine(BUILDTIME_STR + bt.str());
  for(NamedPkgRelVector::const_iterator it = pkgFile.provides.begin();it != pkgFile.provides.end();it++)
    {
      /*
       * The following operation must be done in both cases: in filtering by
       * references mode and without filtering. If there is no filtering we just
       * saving all provides, if filtering is enabled we will proceed real
       * filtering on additional phase.
       */
      m_tmpFile->writeLine(PROVIDES_STR + saveNamedPkgRel(*it));
    }
  for(StringList::const_iterator it = fileList.begin();it != fileList.end();it++)
    /*
     * If filtering by references is enabled we are writing all possible
     * provides to filter them on additional phase. If filterProvidesByDirs
     * string list is empty it means filtering by directories is disabled and
     * we also must write current file as provides. If filtering by
     * directories is enabled we are writing file as provides only if its
     * directory presents in directory list.
     */
    if (m_filterProvidesByRefs || m_filterProvidesByDirs.empty() || fileFromDirs(*it, m_filterProvidesByDirs))
      {
	m_tmpFile->writeLine(PROVIDES_STR + saveFileName(*it));
      }
  for(NamedPkgRelVector::const_iterator it = pkgFile.requires.begin();it != pkgFile.requires.end();it++)
    {
      if (m_requireFilter.excludeRequire(it->pkgName))
	continue;
      m_tmpFile->writeLine(REQUIRES_STR + saveNamedPkgRel(*it));
      if (m_filterProvidesByRefs)
	m_refsSet.insert(it->pkgName);
    }
  for(NamedPkgRelVector::const_iterator it = pkgFile.conflicts.begin();it != pkgFile.conflicts.end();it++)
    {
      m_tmpFile->writeLine(CONFLICTS_STR + saveNamedPkgRel(*it));
      if (m_filterProvidesByRefs)
	m_refsSet.insert(it->pkgName);
    }
  for(NamedPkgRelVector::const_iterator it = pkgFile.obsoletes.begin();it != pkgFile.obsoletes.end();it++)
    m_tmpFile->writeLine(OBSOLETES_STR + saveNamedPkgRel(*it));
  if (m_params.changeLogBinary)
    for(ChangeLog::size_type i = 0;i < pkgFile.changeLog.size();i++)
      m_tmpFile->writeLine(CHANGELOG_STR + encodeChangeLogEntry(pkgFile.changeLog[i]));
  m_tmpFile->writeLine("");
}

void TextFormatWriter::addSource(const PkgFile& pkgFile)
{
  m_sourcesFile->writeLine("[" + File::baseName(pkgFile.fileName) + "]");
  m_sourcesFile->writeLine(NAME_STR + pkgFile.name);
  std::ostringstream epochStr;
  epochStr << EPOCH_STR << pkgFile.epoch;
  m_sourcesFile->writeLine(epochStr.str());
  m_sourcesFile->writeLine(VERSION_STR + pkgFile.version);
  m_sourcesFile->writeLine(RELEASE_STR + pkgFile.release);
  //No nned need to write arch for source packages;
  m_sourcesFile->writeLine(URL_STR + pkgFile.url);
  m_sourcesFile->writeLine(LICENSE_STR + pkgFile.license);
  m_sourcesFile->writeLine(PACKAGER_STR + pkgFile.packager);
  m_sourcesFile->writeLine(SUMMARY_STr + pkgFile.summary);
  m_sourcesFile->writeLine(DESCRIPTION_STR + encodeMultiline(pkgFile.description));
  //No need to write src.rpm entry, usually it is empty for source packages;
  if (m_params.changeLogSources)
    for(ChangeLog::size_type i = 0;i < pkgFile.changeLog.size();i++)
      m_sourcesFile->writeLine(CHANGELOG_STR + encodeChangeLogEntry(pkgFile.changeLog[i]));
  m_sourcesFile->writeLine("");
}

void TextFormatWriter::commitBinary()
{
  m_tmpFile->close();
  if (m_filterProvidesByRefs)
    {
      secondPhase();
    }
  logMsg(LOG_DEBUG, "Removing \'%s\'", m_tmpFileName.c_str());
  File::unlink(m_tmpFileName);
}

void TextFormatWriter::commitSource()
{
  m_sourcesFile->close();
}

void TextFormatWriter::secondPhase()
{
  assert(m_filterProvidesByRefs);
  const std::string inputFileName = m_tmpFileName, outputFileName = m_packagesFileName;
  std::auto_ptr<AbstractTextFileReader> inputFile = createTextFileReader(TextFileStd, inputFileName);
  std::auto_ptr<AbstractTextFileWriter> outputFile = createTextFileWriter(TextFileStd, outputFileName);
  std::string name, line;
  while (inputFile->readLine(line))
    {
      std::string tail;
      if (stringBegins(line, NAME_STR, tail))
	{
	  name = tail;
	  outputFile->writeLine(line);
	  continue;
	}
      if (!stringBegins(line, PROVIDES_STR, tail))
	{
	  outputFile->writeLine(line);
	  continue;
	}
      const std::string provideName = getPkgRelName(tail);
      assert(!provideName.empty());
      if (m_collectedRefs.find(provideName) != m_collectedRefs.end() || 
	  m_additionalRefs.find(provideName) != m_additionalRefs.end() ||
	  (!m_filterProvidesByDirs.empty() && fileFromDirs(provideName, m_filterProvidesByDirs)))
	{
	  outputFile->writeLine(line);
	}
    } //while();
  inputFile->close();
  outputFile->close();
  logMsg(LOG_DEBUG, "Removing \'%s\'", inputFileName.c_str());
  File::unlink(inputFileName);
}

int TextFormatWriter::mapTextFileType(int compressionType)
{
  switch(compressionType)
    {
    case RepoIndexParams::CompressionTypeNone:
      return TextFileStd;
    case RepoIndexParams::CompressionTypeGzip:
      return TextFileGZip;
    default:
      assert(0);
    }; //switch();
  return 0;//Just to reduce warning messages;
}

std::string TextFormatWriter::addCompressionExtension(const std::string& fileName, const RepoIndexParams& params)
{
  if (params.compressionType == RepoIndexParams::CompressionTypeGzip)
    return fileName + COMPRESSION_SUFFIX_GZIP;
  assert(params.compressionType == RepoIndexParams::CompressionTypeNone);
  return fileName;
}

std::string TextFormatWriter::getPkgRelName(const std::string& line)
{
  //Name is stored at the beginning of line until first space without previously used backslash;
  std::string res;
  for(std::string::size_type i = 0;i < line.length();i++)
    {
      if (line[i] == '\\')
	{
	  i++;
	  if (i < line.length())
	    res += line[i]; else 
	    return res + "\\";
	  continue;
	}
      if (line[i] == ' ')
	return res;
      res += line[i];
    } //for();
  return res;
}

std::string TextFormatWriter::saveNamedPkgRel(const NamedPkgRel& r)
{
  std::string name;
  for(std::string::size_type i = 0;i < r.pkgName.length();i++)
    {
      if (r.pkgName[i] == ' ' || r.pkgName[i] == '\\')
	name += "\\";
      name += r.pkgName[i];
    }
  std::ostringstream s;
  s << name;
  if (r.ver.empty())
    return s.str();
  const bool less = r.type & VerLess, equals = r.type & VerEquals, greater = r.type & VerGreater;
  assert(!less || !greater);
  std::string t;
  if (less)
    t += "<";
  if (greater)
    t += ">";
  if (equals)
    t += "=";
  s << " " << t << " " << r.ver;
  return s.str();
}

std::string TextFormatWriter::saveFileName(const std::string& fileName)
{
  std::string s;
  for(std::string::size_type i = 0;i < fileName.length();i++)
    {
      if (fileName[i] == ' ' || fileName[i] == '\\')
	s += "\\";
      s += fileName[i];
    }
  return s;
}

std::string TextFormatWriter::encodeMultiline(const std::string& s)
{
  std::string r;
  for(std::string::size_type i = 0;i < s.length();i++)
    {
      switch(s[i])
	{
	case '\\':
	  r += "\\\\";
	  break;
	case '\n':
	  r += "\\n";
	  break;
	case '\r':
	  continue;
	default:
	  r += s[i];
	}; //switch(s[i]);
    }
  return r;
}

std::string TextFormatWriter::encodeChangeLogEntry(const ChangeLogEntry& entry)
{
  struct tm brTime;
  gmtime_r(&entry.time, &brTime);
  std::ostringstream s;
  s << (brTime.tm_year + 1900) << "-";
  if (brTime.tm_mon < 10)
    s << "0";
  s << (brTime.tm_mon + 1) << "-";
  if (brTime.tm_mday < 10)
    s << "0";
  s << brTime.tm_mday << std::endl;
  s << entry.name << std::endl;
  s << entry.text;
  return encodeMultiline(s.str());
}

bool TextFormatWriter::fileFromDirs(const std::string& fileName, const StringList& dirs)
{
  std::string tail;
  for(StringList::const_iterator it = dirs.begin();it != dirs.end();it++)
    {
      if (stringBegins(fileName, *it, tail))
	return 1;
    }
  return 0;
}
