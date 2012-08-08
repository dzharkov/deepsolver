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

#define TMP_FILE "tmp_packages_data"


TextFormatWriter::TextFormatWriter(AbstractIndexConstructionListener& listener,
				   const AbstractRequireFilter& requireFilter,
				   const RepoIndexParams& params,
				   const std::string& dir,
				   const StringSet& additionalRefs)
  : m_listener(listener),
    m_requireFilter(requireFilter),
    m_params(params),
    m_dir(dir),

    m_tmpFileName(Directory::mixNameComponents(dir, TMP_FILE)),
    m_packagesFileName(addCompressionExtension(Directory::mixNameComponents(dir, REPO_INDEX_PACKAGES_FILE), params)),
    m_packagesDescrFileName(addCompressionExtension(Directory::mixNameComponents(dir, REPO_INDEX_PACKAGES_DESCR_FILE), params)),
    m_sourcesFileName(addCompressionExtension(Directory::mixNameComponents(dir, REPO_INDEX_SOURCE_FILE), params)),

    m_sourcesDescrFileName(addCompressionExtension(Directory::mixNameComponents(dir, REPO_INDEX_SOURCES_DESCR_FILE), params)),
    m_filterProvidesByRefs(params.provideFilteringByRefs),
    m_additionalRefs(additionalRefs),
    m_filterProvidesByDirs(params.provideFilterDirs)
{
}

void TextFormatWriter::initBinary()
{
  if (m_filterProvidesByRefs)//It means the second phase is needed;
    m_packagesFile = createTextFileWriter(TextFileStd, m_tmpFileName); else
    m_packagesFile = createTextFileWriter(mapTextFileType(m_params.compressionType), m_packagesFileName);
    m_packagesDescrFile = createTextFileWriter(mapTextFileType(m_params.compressionType), m_packagesDescrFileName);
}

void TextFormatWriter::initSource()
{
  m_sourcesFile = createTextFileWriter(mapTextFileType(m_params.compressionType), m_sourcesFileName);
  m_sourcesDescrFile = createTextFileWriter(mapTextFileType(m_params.compressionType), m_sourcesDescrFileName);
}

void TextFormatWriter::addBinary(const PkgFile& pkgFile, const StringList& fileList)
{
  m_packagesFile->writeLine("[" + File::baseName(pkgFile.fileName) + "]");
  m_packagesDescrFile->writeLine("[" + File::baseName(pkgFile.fileName) + "]");
  m_packagesFile->writeLine(NAME_STR + pkgFile.name);
  std::ostringstream epochStr;
  epochStr << EPOCH_STR << pkgFile.epoch;
  m_packagesFile->writeLine(epochStr.str());
  m_packagesFile->writeLine(VERSION_STR + pkgFile.version);
  m_packagesFile->writeLine(RELEASE_STR + pkgFile.release);
  m_packagesFile->writeLine(ARCH_STR + pkgFile.arch);
  m_packagesDescrFile->writeLine(URL_STR + pkgFile.url);
  m_packagesDescrFile->writeLine(LICENSE_STR + pkgFile.license);
  m_packagesDescrFile->writeLine(PACKAGER_STR + pkgFile.packager);
  m_packagesDescrFile->writeLine(SUMMARY_STr + pkgFile.summary);
  m_packagesDescrFile->writeLine(DESCRIPTION_STR + encodeMultiline(pkgFile.description));
  m_packagesFile->writeLine(SRCRPM_STR + pkgFile.srcRpm);
  std::ostringstream bt;
  bt << pkgFile.buildTime;
  m_packagesFile->writeLine(BUILDTIME_STR + bt.str());
  for(NamedPkgRelVector::const_iterator it = pkgFile.provides.begin();it != pkgFile.provides.end();it++)
    {
      /*
       * The following operation must be done in both cases: in filtering by
       * references mode and without filtering. If there is no filtering we just
       * saving all provides, if filtering is enabled we will proceed real
       * filtering on additional phase.
       */
      m_packagesFile->writeLine(PROVIDES_STR + saveNamedPkgRel(*it));
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
      m_packagesFile->writeLine(PROVIDES_STR + saveFileName(*it));
  for(NamedPkgRelVector::const_iterator it = pkgFile.requires.begin();it != pkgFile.requires.end();it++)
    {
      if (m_requireFilter.excludeRequire(it->pkgName))
	continue;
      m_packagesFile->writeLine(REQUIRES_STR + saveNamedPkgRel(*it));
      if (m_filterProvidesByRefs)
	m_refsSet.insert(it->pkgName);
    }
  for(NamedPkgRelVector::const_iterator it = pkgFile.conflicts.begin();it != pkgFile.conflicts.end();it++)
    {
      m_packagesFile->writeLine(CONFLICTS_STR + saveNamedPkgRel(*it));
      if (m_filterProvidesByRefs)
	m_refsSet.insert(it->pkgName);
    }
  for(NamedPkgRelVector::const_iterator it = pkgFile.obsoletes.begin();it != pkgFile.obsoletes.end();it++)
    m_packagesFile->writeLine(OBSOLETES_STR + saveNamedPkgRel(*it));
  if (m_params.changeLogBinary)
    for(ChangeLog::size_type i = 0;i < pkgFile.changeLog.size();i++)
      m_packagesDescrFile->writeLine(CHANGELOG_STR + encodeChangeLogEntry(pkgFile.changeLog[i]));
  m_packagesFile->writeLine("");
  m_packagesDescrFile->writeLine("");
}

void TextFormatWriter::addSource(const PkgFile& pkgFile)
{
  m_sourcesFile->writeLine("[" + File::baseName(pkgFile.fileName) + "]");
  m_sourcesDescrFile->writeLine("[" + File::baseName(pkgFile.fileName) + "]");
  m_sourcesFile->writeLine(NAME_STR + pkgFile.name);
  std::ostringstream epochStr;
  epochStr << EPOCH_STR << pkgFile.epoch;
  m_sourcesFile->writeLine(epochStr.str());
  m_sourcesFile->writeLine(VERSION_STR + pkgFile.version);
  m_sourcesFile->writeLine(RELEASE_STR + pkgFile.release);
  //No need need to write arch for source packages;
  m_sourcesDescrFile->writeLine(URL_STR + pkgFile.url);
  m_sourcesDescrFile->writeLine(LICENSE_STR + pkgFile.license);
  m_sourcesDescrFile->writeLine(PACKAGER_STR + pkgFile.packager);
  m_sourcesDescrFile->writeLine(SUMMARY_STr + pkgFile.summary);
  m_sourcesDescrFile->writeLine(DESCRIPTION_STR + encodeMultiline(pkgFile.description));
  //No need to write src.rpm entry, usually it is empty for source packages;
  if (m_params.changeLogSources)
    for(ChangeLog::size_type i = 0;i < pkgFile.changeLog.size();i++)
      m_sourcesDescrFile->writeLine(CHANGELOG_STR + encodeChangeLogEntry(pkgFile.changeLog[i]));
  m_sourcesFile->writeLine("");
  m_sourcesDescrFile->writeLine("");
}

void TextFormatWriter::commitBinary()
{
  m_packagesFile->close();
  m_packagesDescrFile->close();
  if (m_filterProvidesByRefs)
    secondPhase();
}

void TextFormatWriter::commitSource()
{
  m_sourcesFile->close();
  m_sourcesDescrFile->close();
}

void TextFormatWriter::secondPhase()
{
  assert(m_filterProvidesByRefs);
  const std::string inputFileName = m_tmpFileName, outputFileName = m_packagesFileName;
  std::auto_ptr<AbstractTextFileReader> inputFile = createTextFileReader(TextFileStd, inputFileName);
  std::auto_ptr<AbstractTextFileWriter> outputFile = createTextFileWriter(TextFileStd, outputFileName);//FIXME:map;
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
