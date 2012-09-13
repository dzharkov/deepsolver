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
#include"AbstractPackageBackEnd.h"
#include"repo/PkgSection.h"
#include"repo/TextFormatSectionReader.h"
#include"utils/Md5File.h"
#include"utils/GzipInterface.h"

#define TMP_FILE_NAME "tmp_data"

class UnifiedOutput
{
public:
  UnifiedOutput() {}
  virtual ~UnifiedOutput() {}

public:
  virtual void writeData(const std::string& str) = 0;
  virtual void close() = 0;
}; //class UnifiedOutput;

class StdOutput: public UnifiedOutput
{
public:
  StdOutput() {}

  StdOutput(const std::string& fileName)
  {
    open(fileName);
  }

  virtual ~StdOutput() 
  {
    close();
  }

public:
  void open(const std::string& fileName)
  {
    assert(!m_stream.is_open());
    assert(!fileName.empty());
    m_stream.open(fileName.c_str());
    assert(m_stream.is_open());//FIXME:Must be an exception;
  }

  void writeData(const std::string& str)
  {
    assert(m_stream.is_open());
    m_stream << str;
  }

  void close()
  {
    if (!m_stream.is_open())
      return;
    m_stream << std::endl;
    m_stream.flush();
    m_stream.close();
  }

private:
  std::ofstream m_stream;
}; //class StdOutput;

class GzipOutput: public UnifiedOutput
{
public:
  GzipOutput() {}

  GzipOutput(const std::string& fileName)
  {
    open(fileName);
  }

  virtual ~GzipOutput() 
  {
    close();
  }

public:
  void open(const std::string& fileName)
  {
    m_file.open(fileName);
  }

  void writeData(const std::string& str)
  {
    m_file.write(str.c_str(), str.length());
  }

  void close()
  {
    m_file.close();
  }

private:
  GzipOutputFile m_file;
}; //class GzipOutput;

static std::string compressionExtension(char compressionType);
static bool fileFromDirs(const std::string& fileName, const StringVector& dirs);
static std::auto_ptr<AbstractTextFormatSectionReader> createRebuildReader(const std::string& fileName, const RepoParams& params);
static std::auto_ptr<UnifiedOutput> createRebuildWriter(const std::string& fileName, const RepoParams& params);




void IndexCore::buildIndex(const RepoParams& params)
{
  //FIXME:Check target directory is empty;
  assert(!params.indexPath.empty());
  assert(!params.pkgSources.empty());
  Directory::ensureExists(params.indexPath);
  logMsg(LOG_DEBUG, "Starting index creation in \'%s\', target directory exists and empty", params.indexPath.c_str());
  params.writeInfoFile(Directory::mixNameComponents(params.indexPath, REPO_INDEX_INFO_FILE));
  StringSet internalProvidesRefs, externalProvidesRefs;
  if (params.filterProvidesByRefs)
    {
      logMsg(LOG_DEBUG, "Provides filtering by references is enabled, has %zu sources of external references", params.providesRefsSources.size());
      for(StringVector::size_type i = 0;i < params.providesRefsSources.size();i++)
	{
	  m_listener.onReferenceCollecting(params.providesRefsSources[i]);
	  collectRefs(params.providesRefsSources[i], externalProvidesRefs);
	}
      logMsg(LOG_DEBUG, "Has %zu external provides references", externalProvidesRefs.size());
    } else 
    {
      logMsg(LOG_DEBUG, "Provides filtering by references is disabled, skipping collecting of external  references");
      if (!params.providesRefsSources.empty())
	logMsg(LOG_WARNING, "Provides filtering by references is disabled, but parameters have %zu external references sources", params.providesRefsSources.size());
    }
  const std::string pkgFileName = Directory::mixNameComponents(params.indexPath, REPO_INDEX_PACKAGES_FILE + compressionExtension(params.compressionType));
  const std::string pkgDescrFileName = Directory::mixNameComponents(params.indexPath, REPO_INDEX_PACKAGES_DESCR_FILE + compressionExtension(params.compressionType));
  const std::string srcFileName = Directory::mixNameComponents(params.indexPath, REPO_INDEX_SOURCES_FILE + compressionExtension(params.compressionType));
  const std::string srcDescrFileName = Directory::mixNameComponents(params.indexPath, REPO_INDEX_SOURCES_DESCR_FILE + compressionExtension(params.compressionType));
  const std::string pkgFileListFileName = Directory::mixNameComponents(params.indexPath, REPO_INDEX_PACKAGES_FILELIST_FILE + compressionExtension(params.compressionType));
  const std::string pkgCompleteFileName = Directory::mixNameComponents(params.indexPath, REPO_INDEX_PACKAGES_COMPLETE_FILE);
  std::auto_ptr<UnifiedOutput> pkgFile, pkgDescrFile, pkgFileListFile, pkgCompleteFile, srcFile, srcDescrFile;
  if (params.compressionType == RepoParams::CompressionTypeGzip)
    {
      pkgFile = std::auto_ptr<UnifiedOutput>(new GzipOutput(pkgFileName));
      pkgDescrFile = std::auto_ptr<UnifiedOutput>(new GzipOutput(pkgDescrFileName));
      pkgFileListFile = std::auto_ptr<UnifiedOutput>(new GzipOutput(pkgFileListFileName));
      srcFile = std::auto_ptr<UnifiedOutput>(new GzipOutput(srcFileName));
      srcDescrFile = std::auto_ptr<UnifiedOutput>(new GzipOutput(srcDescrFileName));
    } else 
    {
      assert(params.compressionType == RepoParams::CompressionTypeNone);
      pkgFile = std::auto_ptr<UnifiedOutput>(new StdOutput(pkgFileName));
      pkgDescrFile = std::auto_ptr<UnifiedOutput>(new StdOutput(pkgDescrFileName));
      pkgFileListFile = std::auto_ptr<UnifiedOutput>(new StdOutput(pkgFileListFileName));
      srcFile = std::auto_ptr<UnifiedOutput>(new StdOutput(srcFileName));
      srcDescrFile = std::auto_ptr<UnifiedOutput>(new StdOutput(srcDescrFileName));
    }
      pkgCompleteFile = std::auto_ptr<UnifiedOutput>(new StdOutput(pkgCompleteFileName));
  logMsg(LOG_DEBUG, "All files were created");
  std::auto_ptr<AbstractPackageBackEnd> backend = CREATE_PACKAGE_BACKEND;
  logMsg(LOG_DEBUG, "Package backend was created");
  //We ready to collect information about packages in specified directories;
  size_t countBinary = 0, countSource = 0;
  for(StringVector::size_type i = 0;i < params.pkgSources.size();i++)
    {
      logMsg(LOG_INFO, "Reading packages in \'%s\'", params.pkgSources[i].c_str());
      m_listener.onPackageCollecting(params.pkgSources[i]);
      std::auto_ptr<Directory::Iterator> it = Directory::enumerate(params.pkgSources[i]);
      while(it->moveNext())
	{
	  if (it->getName() == "." || it->getName() == "..")
	    continue;
	  if (!backend->validPkgFileName(it->getName()))
	    continue;
	  PkgFile pkg;
	  backend->readPackageFile(it->getFullPath(), pkg);
	  pkg.fileName = it->getName();
	  pkg.isSource = backend->validSourcePkgFileName(it->getName());
	  if (params.filterProvidesByRefs)
	    {
	      for(NamedPkgRelVector::size_type k = 0;k < pkg.requires.size();k++)
		internalProvidesRefs.insert(pkg.requires[k].pkgName);
	      for(NamedPkgRelVector::size_type k = 0;k < pkg.conflicts.size();k++)
		internalProvidesRefs.insert(pkg.conflicts[k].pkgName);
	    }
	  if (!pkg.isSource)
	    {
	      countBinary++;
	      if (!params.filterProvidesByRefs)
		pkgFile->writeData(PkgSection::saveBaseInfo(pkg, params.filterProvidesByRefs?StringVector():params.filterProvidesByDirs));
	      pkgDescrFile->writeData(PkgSection::saveDescr(pkg, params.changeLogBinary));
	      pkgFileListFile->writeData(PkgSection::saveFileList(pkg));
	      pkgCompleteFile->writeData(PkgSection::saveBaseInfo(pkg, StringVector()));
	    } else
	    {
	      countSource++;
	      srcFile->writeData(PkgSection::saveBaseInfo(pkg, params.filterProvidesByRefs?StringVector():params.filterProvidesByDirs));
	      srcDescrFile->writeData(PkgSection::saveDescr(pkg, params.changeLogSources));
	    }
	} //for package files;
      logMsg(LOG_DEBUG, "Directory reading completed, picked up %zu binary packages and %zu source packages so far", countBinary, countSource);
    } //for listed directories;
  if (!params.filterProvidesByRefs)
    pkgFile->close();
  pkgDescrFile->close();
  pkgFileListFile->close();
  pkgCompleteFile->close();
  srcFile->close();
  srcDescrFile->close();
  //Additional phase for provides filtering if needed;
  if (params.filterProvidesByRefs)
    {
    logMsg(LOG_INFO, "Performing additional phase for provides cleaning, has %zu internal provides references and %zu external provides references", internalProvidesRefs.size(), externalProvidesRefs.size());
      m_listener.onProvidesCleaning();
      std::ifstream is(pkgCompleteFileName.c_str());
      if (!is.is_open())
	throw IndexCoreException(IndexErrorInternalIOProblem);
      while(1)
	{
	  std::string line;
	  std::getline(is, line);
	  if (!is)
	    break;
	  std::string pkgName;
	  if (!PkgSection::isProvidesLine(line, pkgName))
	    {
	      pkgFile->writeData(line);
	      continue;
	    }
	  if (internalProvidesRefs.find(pkgName) != internalProvidesRefs.end() || 
	  externalProvidesRefs.find(pkgName) != externalProvidesRefs.end() ||
	  (!params.filterProvidesByDirs.empty() && fileFromDirs(pkgName, params.filterProvidesByDirs)))
	pkgFile->writeData(line);
	} //for(lines);
      pkgFile->close();
      logMsg(LOG_DEBUG, "Provides filtering completed");
    } else 
    logMsg(LOG_DEBUG, "Skipping additional phase for provides filtering");
  logMsg(LOG_DEBUG, "Preparing md5-checksum file");
  m_listener.onChecksumWriting();
  Md5File md5;
  logMsg(LOG_DEBUG, "Registering \'%s\'", Directory::mixNameComponents(params.indexPath, REPO_INDEX_INFO_FILE).c_str());
  md5.addItemFromFile(REPO_INDEX_INFO_FILE, Directory::mixNameComponents(params.indexPath, REPO_INDEX_INFO_FILE));
  logMsg(LOG_DEBUG, "Registering \'%s\'", pkgFileName.c_str());
  md5.addItemFromFile(File::baseName(pkgFileName), pkgFileName);
  logMsg(LOG_DEBUG, "Registering \'%s\'", pkgDescrFileName.c_str());
  md5.addItemFromFile(File::baseName(pkgDescrFileName), pkgDescrFileName);
  logMsg(LOG_DEBUG, "Registering \'%s\'", pkgFileListFileName.c_str());
  md5.addItemFromFile(File::baseName(pkgFileListFileName), pkgFileListFileName);
  logMsg(LOG_DEBUG, "Registering \'%s\'", srcFileName.c_str());
  md5.addItemFromFile(File::baseName(srcFileName), srcFileName);
  logMsg(LOG_DEBUG, "Registering \'%s\'", srcDescrFileName.c_str());
  md5.addItemFromFile(File::baseName(srcDescrFileName), srcDescrFileName);
  logMsg(LOG_INFO, "Writing md5-checksum file");
  md5.saveToFile(Directory::mixNameComponents(params.indexPath, REPO_INDEX_MD5SUM_FILE));
  logMsg(LOG_DEBUG, "Exiting index building procedure, everything done successfully");
}

void IndexCore::rebuildIndex(const RepoParams& params, const StringVector& toAdd, const StringVector& toRemove)
{
  logMsg(LOG_DEBUG, "starting index patching process with %zu items to add and %zu items to remove", toAdd.size(), toRemove.size());
  std::auto_ptr<AbstractPackageBackEnd> backend = CREATE_PACKAGE_BACKEND;
  PkgFileVector pkgs;
  pkgs.resize(toAdd.size());
  for(PkgFileVector::size_type i = 0;i < pkgs.size();i++)
    {
      backend->readPackageFile(toAdd[i], pkgs[i]);
      pkgs[i].fileName = File::baseName(toAdd[i]);
      pkgs[i].isSource = backend->validSourcePkgFileName(toAdd[i]);
    }
  BoolVector skipToAdd;
  skipToAdd.resize(toAdd.size());
  std::string sect;
  std::string inputFileName, outputFileName;
    std::auto_ptr<AbstractTextFormatSectionReader> reader;
  std::auto_ptr<UnifiedOutput> writer;

    //Packages file;
  for(BoolVector::size_type i = 0;i < skipToAdd.size();i++)
    skipToAdd[i] = 0;
    inputFileName = REPO_INDEX_PACKAGES_FILE + compressionExtension(params.compressionType);
  outputFileName = TMP_FILE_NAME + compressionExtension(params.compressionType);
  logMsg(LOG_DEBUG, "Patching \'%s\' to \'%s\'", inputFileName.c_str(), outputFileName.c_str());
  m_listener.onPatchingFile(inputFileName);
  reader = createRebuildReader(inputFileName, params);
  writer = createRebuildWriter(outputFileName, params);
  reader->init();
  while(reader->readNext(sect))
    {
      const std::string fileName = PkgSection::getPkgFileName(sect);
      assert(!fileName.empty());//FIXME:
      for(PkgFileVector::size_type i = 0;i < pkgs.size();i++)
	{
	  if (fileName != pkgs[i].fileName)
	    continue;
	  skipToAdd[i] = 1;
	  logMsg(LOG_WARNING, "File \'%s\' already present in index \'%s\', skipping adding", fileName.c_str(), inputFileName.c_str());
	  m_listener.onNoTwiceAdding(fileName);
	}
      StringVector::size_type i = 0;
      for(i = 0;i < toRemove.size();i++)
	if (fileName == toRemove[i])
	  break;
      if (i >= toRemove.size())
	writer->writeData(sect); else
	logMsg(LOG_DEBUG, "Found package to exclude: \'%s\'", fileName.c_str());
    }
  for(PkgFileVector::size_type i = 0;i < pkgs.size();i++)
    if (!pkgs[i].isSource)
      if (!skipToAdd[i])
	{
	  writer->writeData(PkgSection::saveBaseInfo(pkgs[i], StringVector()));
	  logMsg(LOG_DEBUG, "File \'%s\' added to index", pkgs[i].fileName.c_str());
	}
  reader->close();
  writer->close();

    //Packages descriptions file;
  for(BoolVector::size_type i = 0;i < skipToAdd.size();i++)
    skipToAdd[i] = 0;
    inputFileName = REPO_INDEX_PACKAGES_DESCR_FILE + compressionExtension(params.compressionType);
  outputFileName = TMP_FILE_NAME + compressionExtension(params.compressionType);
  logMsg(LOG_DEBUG, "Patching \'%s\' to \'%s\'", inputFileName.c_str(), outputFileName.c_str());
  m_listener.onPatchingFile(inputFileName);
  reader = createRebuildReader(inputFileName, params);
  writer = createRebuildWriter(outputFileName, params);
  reader->init();
  while(reader->readNext(sect))
    {
      const std::string fileName = PkgSection::getPkgFileName(sect);
      assert(!fileName.empty());//FIXME:
      for(PkgFileVector::size_type i = 0;i < pkgs.size();i++)
	{
	  if (fileName != pkgs[i].fileName)
	    continue;
	  skipToAdd[i] = 1;
	  logMsg(LOG_WARNING, "File \'%s\' already present in index \'%s\', skipping adding", fileName.c_str(), inputFileName.c_str());
	  m_listener.onNoTwiceAdding(fileName);
	}
      StringVector::size_type i = 0;
      for(i = 0;i < toRemove.size();i++)
	if (fileName == toRemove[i])
	  break;
      if (i >= toRemove.size())
	writer->writeData(sect); else
	logMsg(LOG_DEBUG, "Found package to exclude: \'%s\'", fileName.c_str());
    }
  for(PkgFileVector::size_type i = 0;i < pkgs.size();i++)
    if (!pkgs[i].isSource)
      if (!skipToAdd[i])
	{
	  writer->writeData(PkgSection::saveDescr(pkgs[i], params.changeLogBinary));
	  logMsg(LOG_DEBUG, "File \'%s\' added to index", pkgs[i].fileName.c_str());
	}
  reader->close();
  writer->close();

    //Sources file;
  for(BoolVector::size_type i = 0;i < skipToAdd.size();i++)
    skipToAdd[i] = 0;
    inputFileName = REPO_INDEX_SOURCES_FILE + compressionExtension(params.compressionType);
  outputFileName = TMP_FILE_NAME + compressionExtension(params.compressionType);
  logMsg(LOG_DEBUG, "Patching \'%s\' to \'%s\'", inputFileName.c_str(), outputFileName.c_str());
  m_listener.onPatchingFile(inputFileName);
  reader = createRebuildReader(inputFileName, params);
  writer = createRebuildWriter(outputFileName, params);
  reader->init();
  while(reader->readNext(sect))
    {
      const std::string fileName = PkgSection::getPkgFileName(sect);
      assert(!fileName.empty());//FIXME:
      for(PkgFileVector::size_type i = 0;i < pkgs.size();i++)
	{
	  if (fileName != pkgs[i].fileName)
	    continue;
	  skipToAdd[i] = 1;
	  logMsg(LOG_WARNING, "File \'%s\' already present in index \'%s\', skipping adding", fileName.c_str(), inputFileName.c_str());
	  m_listener.onNoTwiceAdding(fileName);
	}
      StringVector::size_type i = 0;
      for(i = 0;i < toRemove.size();i++)
	if (fileName == toRemove[i])
	  break;
      if (i >= toRemove.size())
	writer->writeData(sect); else
	logMsg(LOG_DEBUG, "Found package to exclude: \'%s\'", fileName.c_str());
    }
  for(PkgFileVector::size_type i = 0;i < pkgs.size();i++)
    if (pkgs[i].isSource)
      if (!skipToAdd[i])
	{
	  writer->writeData(PkgSection::saveBaseInfo(pkgs[i], StringVector()));
	  logMsg(LOG_DEBUG, "File \'%s\' added to index", pkgs[i].fileName.c_str());
	}
  reader->close();
  writer->close();

    //Sources descriptions file;
  for(BoolVector::size_type i = 0;i < skipToAdd.size();i++)
    skipToAdd[i] = 0;
    inputFileName = REPO_INDEX_SOURCES_DESCR_FILE + compressionExtension(params.compressionType);
  outputFileName = TMP_FILE_NAME + compressionExtension(params.compressionType);
  logMsg(LOG_DEBUG, "Patching \'%s\' to \'%s\'", inputFileName.c_str(), outputFileName.c_str());
  m_listener.onPatchingFile(inputFileName);
  reader = createRebuildReader(inputFileName, params);
  writer = createRebuildWriter(outputFileName, params);
  reader->init();
  while(reader->readNext(sect))
    {
      const std::string fileName = PkgSection::getPkgFileName(sect);
      assert(!fileName.empty());//FIXME:
      for(PkgFileVector::size_type i = 0;i < pkgs.size();i++)
	{
	  if (fileName != pkgs[i].fileName)
	    continue;
	  skipToAdd[i] = 1;
	  logMsg(LOG_WARNING, "File \'%s\' already present in index \'%s\', skipping adding", fileName.c_str(), inputFileName.c_str());
	  m_listener.onNoTwiceAdding(fileName);
	}
      StringVector::size_type i = 0;
      for(i = 0;i < toRemove.size();i++)
	if (fileName == toRemove[i])
	  break;
      if (i >= toRemove.size())
	writer->writeData(sect); else
	logMsg(LOG_DEBUG, "Found package to exclude: \'%s\'", fileName.c_str());
    }
  for(PkgFileVector::size_type i = 0;i < pkgs.size();i++)
    if (!pkgs[i].isSource)
      if (!skipToAdd[i])
	{
	  writer->writeData(PkgSection::saveDescr(pkgs[i], params.changeLogSources));
	  logMsg(LOG_DEBUG, "File \'%s\' added to index", pkgs[i].fileName.c_str());
	}
  reader->close();
  writer->close();
}

void IndexCore::fixReferences(const RepoParams& params)
{
  /*
  const std::string pkgFileName = Directory::mixNameComponents(params.indexPath, REPO_INDEX_PACKAGES_FILE + compressionExtension(params.compressionType));
  StringSet references;
  if (params.filterProvidesByRefs)
    {
      for(StringVector::size_type i = 0;i < params.providesRefsSources.size();i++)
	collectRefs(references, params.providesRefsSources);
      std::auto_ptr<AbstractTextFormatSectionReader> reader = createRebuildReader(pkgFileName, params);
      std::string sect;
      while(reader->readNext(sect))
	PkgSection::extractProvidesReferences(sect, references);
      reader->close();
    }
  File::unlink(pkgFIleName);
  std::auto_ptr<UnifiedOutput> pkgFile;
  if (params.compressionType == RepoParams::CompressionTypeGzip)
    pkgFile = std::auto_ptr<UnifiedOutput>(new GzipOutput(pkgFileName)); else
    pkgFile = std::auto_ptr<UnifiedOutput>(new StdOutput(tmpFileName));
  //Ready for real filtering;
  //FIXME:  m_listener.onProvidesCleaning();
  std::ifstream is(Directory::mixNameComponents(params.indexPath, REPO_INDEX_PACKAGES_COMPLETE_FILE).c_str());
  if (!is.is_open())
    throw IndexCoreException(IndexErrorInternalIOProblem);
  while(1)
    {
      std::string line;
      std::getline(is, line);
      if (!is)
	break;
      std::string pkgName;
      if (!PkgSection::isProvidesLine(line, pkgName))
	{
	  pkgFile->writeData(line);
	  continue;
	}
      if (references.find(pkgName) != references.end() || 
	  (!params.filterProvidesByDirs.empty() && fileFromDirs(pkgName, params.filterProvidesByDirs)))
	pkgFile->writeData(line);
    } //while(lines);
  pkgFile->close();
  */
}

void IndexCore::collectRefs(const std::string& dirName, StringSet& res) 
{
  logMsg(LOG_DEBUG, "Collecting references in \'%s\'", dirName.c_str());
  //First of all checking if there is already created repo index;
  try {
    logMsg(LOG_DEBUG, "Checking is there repo index");
    RepoParams repoParams;
    repoParams.readInfoFile(Directory::mixNameComponents(dirName, REPO_INDEX_INFO_FILE));
    const std::string pkgFileName = Directory::mixNameComponents(dirName, REPO_INDEX_PACKAGES_FILE) + compressionExtension(repoParams.compressionType);
    std::auto_ptr<AbstractTextFormatSectionReader> reader = createRebuildReader(pkgFileName, repoParams);
    logMsg(LOG_DEBUG, "Creating section reader for reading file \'%s\'", pkgFileName.c_str());
    std::string sect;
    reader->init();
    while(reader->readNext(sect))
      PkgSection::extractProvidesReferences(sect, res);
    reader->close();
    logMsg(LOG_DEBUG, "We successfully read references from repository index, references set contains %zu entries", res.size());
    return;
  }
  catch (const DeepsolverException& e)
    {
      //Repository index reading failed, nothing sad, just making the log report;
      logMsg(LOG_DEBUG, "Directory \'%s\' does not contains a valid repo index:%s error:%s", dirName.c_str(), e.getType().c_str(), e.getMessage().c_str());
    }
  logMsg(LOG_DEBUG, "Since repository index failed  we are looking for packages files");
  std::auto_ptr<AbstractPackageBackEnd> backend = CREATE_PACKAGE_BACKEND;
  std::auto_ptr<Directory::Iterator> it = Directory::enumerate(dirName);
  while(it->moveNext())
    {
      if (it->getName() == "." || it->getName() == "..")
	continue;
      if (!backend->validPkgFileName(it->getName()))
	continue;
      PkgFile pkgFile;
      backend->readPackageFile(it->getFullPath(), pkgFile);
      for(NamedPkgRelVector::size_type i =0; i < pkgFile.requires.size();i++)
	res.insert(pkgFile.requires[i].pkgName);
      for(NamedPkgRelVector::size_type i = 0;i < pkgFile.conflicts.size();i++)
	res.insert(pkgFile.conflicts[i].pkgName);
    }
}

std::string compressionExtension(char compressionType)
{
  if (compressionType == RepoParams::CompressionTypeGzip)
    return COMPRESSION_SUFFIX_GZIP;
  assert(compressionType == RepoParams::CompressionTypeNone);
  return "";
}

bool fileFromDirs(const std::string& fileName, const StringVector& dirs)
{
  std::string tail;
  for(StringVector::const_iterator it = dirs.begin();it != dirs.end();it++)
    if (stringBegins(fileName, *it, tail))
      return 1;
  return 0;
}

std::auto_ptr<AbstractTextFormatSectionReader> createRebuildReader(const std::string& fileName, const RepoParams& params)
{
  if (params.compressionType == RepoParams::CompressionTypeNone)
    {
      std::auto_ptr<TextFormatSectionReader> reader(new TextFormatSectionReader());
      reader->open(fileName);
      return std::auto_ptr<AbstractTextFormatSectionReader>(reader.release());
    }
  if (params.compressionType == RepoParams::CompressionTypeGzip)
    {
      std::auto_ptr<TextFormatSectionReaderGzip> reader(new TextFormatSectionReaderGzip());
      reader->open(fileName);
      return std::auto_ptr<AbstractTextFormatSectionReader>(reader.release());
    }
  assert(0);
  return std::auto_ptr<AbstractTextFormatSectionReader>();
}

std::auto_ptr<UnifiedOutput> createRebuildWriter(const std::string& fileName, const RepoParams& params)
{
  if (params.compressionType == RepoParams::CompressionTypeNone)
    return std::auto_ptr<UnifiedOutput>(new StdOutput(fileName));
  if (params.compressionType == RepoParams::CompressionTypeGzip)
    return std::auto_ptr<UnifiedOutput>(new GzipOutput(fileName));
  assert(0);
  return std::auto_ptr<UnifiedOutput>();
}
