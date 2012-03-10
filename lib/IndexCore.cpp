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
#include"IndexCore.h"
#include"RepoIndexInfoFile.h"
#include"RepoIndexTextFormatWriter.h"
#include"rpm/RpmFile.h"
#include"rpm/RpmFileHeaderReader.h"
#include"MD5.h"
#include"RequireFilter.h"

void IndexCore::collectRefs(const std::string& dirName, StringSet& res) 
{
  m_console.msg() << "Collecting additional references from " << dirName << "...";
  std::auto_ptr<Directory::Iterator> it = Directory::enumerate(dirName);
  while(it->moveNext())
    {
      if (it->getName() == "." || it->getName() == "..")
	continue;
      if (!checkExtension(it->getName(), ".rpm"))
	continue;
      NamedPkgRelVector requires, conflicts;
      RpmFileHeaderReader reader;
      reader.load(it->getFullPath());
      reader.fillRequires(requires);
      reader.fillConflicts(conflicts);
      for(NamedPkgRelVector::size_type i =0; i < requires.size();i++)
	res.insert(requires[i].pkgName);
      for(NamedPkgRelVector::size_type i = 0;i < conflicts.size();i++)
	res.insert(conflicts[i].pkgName);
    }
  m_console.msg() << " " << res.size() << " found!" << std::endl;
}

void IndexCore::collectRefsFromDirs(const StringList& dirs, StringSet& res)
{
  res.clear();
  for(StringList::const_iterator it = dirs.begin();it != dirs.end();it++)
    collectRefs(*it, res);
}

void IndexCore::build(const RepoIndexParams& params)
{
  const std::string archDir = concatUnixPath(params.topDir, params.arch);
  const std::string indexDir = concatUnixPath(archDir, REPO_INDEX_DIR);
  const std::string infoFile = concatUnixPath(indexDir, REPO_INDEX_INFO_FILE);
  Directory::ensureExists(indexDir);
  writeInfoFile(infoFile, params);
  processPackages(indexDir,
		  concatUnixPath(archDir, REPO_RPMS_DIR_NAME),
		  concatUnixPath(archDir, REPO_SRPMS_DIR_NAME),
		  params);
}

void IndexCore::processPackages(const std::string& indexDir, const std::string& rpmsDir, const std::string& srpmsDir, const RepoIndexParams& params)
{
  assert(params.formatType == RepoIndexParams::FormatTypeText);//FIXME:binary format support;
  logMsg(LOG_DEBUG, "IndexCore began processing directories with packages");
  StringSet additionalRefs;
  if (params.provideFilteringByRefs)
    {
      logMsg(LOG_DEBUG, "Provide filtering by used references is enabled, checking additional directories with packages to collect references entries");
      collectRefsFromDirs(params.takeRefsFromPackageDirs, additionalRefs);
    }
  RequireFilter requireFilter;
  if (!params.excludeRequiresFile.empty())
    requireFilter.load(params.excludeRequiresFile);
  RepoIndexTextFormatWriter handler(requireFilter, params, m_console, indexDir, additionalRefs);
  //Binary packages;
  handler.initBinary();
  logMsg(LOG_DEBUG, "RepoIndexTextFormatWriter created and initialized for binary packages");
  m_console.msg() << "Looking through " << rpmsDir << " to pick packages for repository index...";
  std::auto_ptr<Directory::Iterator> it = Directory::enumerate(rpmsDir);
  logMsg(LOG_DEBUG, "Created directory iterator for enumerating \'%s\'", rpmsDir.c_str());
  size_t count = 0;
  while(it->moveNext())
    {
      if (it->getName() == "." || it->getName() == "..")
	continue;
      if (!checkExtension(it->getName(), ".rpm"))
	continue;
      PkgFile pkgFile;
      StringList files;
      readRpmPkgFile(it->getFullPath(), pkgFile, files);
      handler.addBinary(pkgFile, files);
      count++;
    }
  m_console.msg() << " picked up " << count << " binary packages!" << std::endl;
  handler.commitBinary();
  logMsg(LOG_DEBUG, "Committed %zu binary packages", count);
  //Source packages;
  logMsg(LOG_DEBUG, "Binary packages processing is finished, switching to sources");
  m_console.msg() << "Looking through " << srpmsDir << " to pick source packages...";
  it = Directory::enumerate(srpmsDir);
  logMsg(LOG_DEBUG, "Created directory iterator for enumerating \'%s\'", srpmsDir.c_str());
  count = 0;
  handler.initSource();
  logMsg(LOG_DEBUG, "RepoIndexTextFormatWriter initialized for source packages");
  while(it->moveNext())
    {
      if (it->getName() == "." || it->getName() == "..")
	continue;
      if (!checkExtension(it->getName(), ".src.rpm"))
	continue;
      PkgFile pkgFile;
      StringList files;
      readRpmPkgFile(it->getFullPath(), pkgFile, files);
      handler.addSource(pkgFile);
      count++;
    }
  m_console.msg() << " picked up " << count << " source packages!" << std::endl;
  logMsg(LOG_DEBUG, "Committed %zu source packages", count);
  handler.commitSource();
  //Saving md5sum;
  m_console.msg() << "Creating " << REPO_INDEX_MD5SUM_FILE << "...";
  std::auto_ptr<AbstractTextFileWriter> md5sum = createTextFileWriter(TextFileStd, concatUnixPath(indexDir, REPO_INDEX_MD5SUM_FILE));
  MD5 md5;
  md5.init();
  md5.updateFromFile(concatUnixPath(indexDir, REPO_INDEX_INFO_FILE));
  md5sum->writeLine(md5.commit(REPO_INDEX_INFO_FILE));
  md5.init();
  md5.updateFromFile(handler.getRpmsFileName());
  md5sum->writeLine(md5.commit(File::baseName(handler.getRpmsFileName())));
  md5.init();
  md5.updateFromFile(handler.getSrpmsFileName());
  md5sum->writeLine(md5.commit(File::baseName(handler.getSrpmsFileName())));
  md5.init();
  md5.updateFromFile(handler.getProvidesFileName());
  md5sum->writeLine(md5.commit(File::baseName(handler.getProvidesFileName())));
  m_console.msg() << " OK!" << std::endl;
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
  m_console.msg() << "Created repository index info file " << fileName << std::endl;
}
