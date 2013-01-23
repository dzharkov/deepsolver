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
#include"IndexCore.h"
#include"AbstractPackageBackEnd.h"
#include"repo/PkgSection.h"
#include"repo/TextFormatSectionReader.h"
#include"utils/Md5File.h"
#include"utils/GzipInterface.h"
#include"utils/RegExp.h"

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
    if (!m_stream.is_open())
      throw IndexCoreException(IndexCoreException::InternalIOProblem);
    logMsg(LOG_DEBUG, "Std file output for \'%s\' is opened", fileName.c_str());
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

class RegExpCollection
{
public:
  RegExpCollection() {}
  virtual ~RegExpCollection() {}

public:
  void fill(const StringVector& exps)
  {
    m_regExps.clear();
    m_regExps.resize(exps.size());
    for(StringVector::size_type i = 0;i < exps.size();i++)
      {
	assert(!exps[i].empty());
	m_regExps[i].compile(exps[i]);
      }
  }

  bool matchAtLeastOne(const std::string& line) const
  {
    for(RegExpVector::size_type i = 0;i < m_regExps.size();i++)
      if (m_regExps[i].match(line))
	return 1;
    return 0;
  }

private:
  RegExpVector m_regExps;
}; //class RegExpCollection;

static std::string compressionExtension(char compressionType);
static bool fileFromDirs(const std::string& fileName, const StringVector& dirs);
static std::auto_ptr<AbstractTextFormatSectionReader> createRebuildReader(const std::string& fileName, const RepoParams& params);
static std::auto_ptr<AbstractTextFormatSectionReader> createRebuildReaderNoCompression(const std::string& fileName);
static std::auto_ptr<UnifiedOutput> createRebuildWriter(const std::string& fileName, const RepoParams& params);




void IndexCore::buildIndex(const RepoParams& params)
{
  assert(!params.indexPath.empty());
  assert(!params.pkgSources.empty());
  Directory::ensureExists(params.indexPath);
  if (!Directory::empty(params.indexPath))
    throw IndexCoreException(IndexCoreException::DirectoryNotEmpty, params.indexPath);
  logMsg(LOG_DEBUG, "Starting index creation in \'%s\', target directory exists and empty", params.indexPath.c_str());
  RegExpCollection regExps;
  regExps.fill(params.excludeRequiresRegExp);
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
	  if (it->name() == "." || it->name() == "..")
	    continue;
	  if (!backend->validPkgFileName(it->name()))
	    continue;
	  PkgFile pkg;
	  backend->readPackageFile(it->fullPath(), pkg);
	  pkg.fileName = it->name();
	  pkg.isSource = backend->validSourcePkgFileName(it->name());
	  NamedPkgRelVector requires = pkg.requires;
	  pkg.requires.clear();
	  for(NamedPkgRelVector::size_type i = 0;i < requires.size();i++)
	    if (!regExps.matchAtLeastOne(requires[i].pkgName))
	      pkg.requires.push_back(requires[i]);
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
	throw IndexCoreException(IndexCoreException::InternalIOProblem);
      while(1)
	{
	  std::string line;
	  std::getline(is, line);
	  if (!is)
	    break;
	  std::string pkgName;
	  if (!PkgSection::isProvidesLine(line, pkgName))
	    {
	      pkgFile->writeData(line + "\n");
	      continue;
	    }
	  if (internalProvidesRefs.find(pkgName) != internalProvidesRefs.end() || 
	  externalProvidesRefs.find(pkgName) != externalProvidesRefs.end() ||
	  (!params.filterProvidesByDirs.empty() && fileFromDirs(pkgName, params.filterProvidesByDirs)))
	pkgFile->writeData(line + "\n");
	} //for(lines);
      pkgFile->close();
      logMsg(LOG_DEBUG, "Provides filtering completed");
    } else 
    logMsg(LOG_DEBUG, "Skipping additional phase for provides filtering");
  logMsg(LOG_DEBUG, "Preparing md5sum file");
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
  logMsg(LOG_INFO, "Writing md5sum file");
  md5.saveToFile(Directory::mixNameComponents(params.indexPath, REPO_INDEX_MD5SUM_FILE));
  logMsg(LOG_DEBUG, "Exiting index building procedure, everything done successfully");
}

void IndexCore::rebuildIndex(const RepoParams& params, const StringVector& toAdd, const StringVector& toRemove)
{
  logMsg(LOG_DEBUG, "starting index patching process with %zu items to add and %zu items to remove", toAdd.size(), toRemove.size());
  RegExpCollection regExps;
  regExps.fill(params.excludeRequiresRegExp);
  m_listener.onChecksumVerifying();
  if (params.md5sumFileName.empty())
    throw IndexCoreException(IndexCoreException::MissedChecksumFileName);
  logMsg(LOG_INFO, "Verifying md5sums");
  Md5File md5File;
  md5File.loadFromFile(Directory::mixNameComponents(params.indexPath, params.md5sumFileName));
  for(Md5File::ItemVector::size_type i = 0;i < md5File.items.size();i++)
    {
      logMsg(LOG_DEBUG, "Verifying checksum for \'%s\'", Directory::mixNameComponents(params.indexPath, md5File.items[i].fileName).c_str());
      if (!md5File.verifyItem(i, Directory::mixNameComponents(params.indexPath, md5File.items[i].fileName)))
	throw IndexCoreException(IndexCoreException::CorruptedFile, Directory::mixNameComponents(params.indexPath, md5File.items[i].fileName));
    }
  std::auto_ptr<AbstractPackageBackEnd> backend = CREATE_PACKAGE_BACKEND;
  PkgFileVector pkgs;
  pkgs.resize(toAdd.size());
  logMsg(LOG_INFO, "Reading packages to add (%zu total)", pkgs.size());
  for(PkgFileVector::size_type i = 0;i < pkgs.size();i++)
    {
      backend->readPackageFile(toAdd[i], pkgs[i]);
      pkgs[i].fileName = File::baseName(toAdd[i]);
      pkgs[i].isSource = backend->validSourcePkgFileName(toAdd[i]);
      NamedPkgRelVector requires = pkgs[i].requires;
      pkgs[i].requires.clear();
      for(NamedPkgRelVector::size_type j = 0;j < requires.size();j++)
	if (!regExps.matchAtLeastOne(requires[j].pkgName))
	  pkgs[i].requires.push_back(requires[j]);
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
  inputFileName = Directory::mixNameComponents(params.indexPath, REPO_INDEX_PACKAGES_FILE + compressionExtension(params.compressionType));
  outputFileName = Directory::mixNameComponents(params.indexPath, TMP_FILE_NAME + compressionExtension(params.compressionType));
  logMsg(LOG_INFO, "Patching \'%s\' to \'%s\'", inputFileName.c_str(), outputFileName.c_str());
  m_listener.onPatchingFile(inputFileName);
  reader = createRebuildReader(inputFileName, params);
  writer = createRebuildWriter(outputFileName, params);
  reader->init();
  while(reader->readNext(sect))
    {
      const std::string fileName = PkgSection::getPkgFileName(sect);
          if(fileName.empty())
	    throw IndexCoreException(IndexCoreException::InternalIOProblem);
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
  File::unlink(inputFileName);
  File::move(outputFileName, inputFileName);

    //Packages descriptions file;
  for(BoolVector::size_type i = 0;i < skipToAdd.size();i++)
    skipToAdd[i] = 0;
  inputFileName = Directory::mixNameComponents(params.indexPath, REPO_INDEX_PACKAGES_DESCR_FILE + compressionExtension(params.compressionType));
  outputFileName = Directory::mixNameComponents(params.indexPath, TMP_FILE_NAME + compressionExtension(params.compressionType));
  logMsg(LOG_INFO, "Patching \'%s\' to \'%s\'", inputFileName.c_str(), outputFileName.c_str());
  m_listener.onPatchingFile(inputFileName);
  reader = createRebuildReader(inputFileName, params);
  writer = createRebuildWriter(outputFileName, params);
  reader->init();
  while(reader->readNext(sect))
    {
      const std::string fileName = PkgSection::getPkgFileName(sect);
      if(fileName.empty())
	throw IndexCoreException(IndexCoreException::InternalIOProblem);
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
  File::unlink(inputFileName);
  File::move(outputFileName, inputFileName);

    //Sources file;
  for(BoolVector::size_type i = 0;i < skipToAdd.size();i++)
    skipToAdd[i] = 0;
  inputFileName = Directory::mixNameComponents(params.indexPath, REPO_INDEX_SOURCES_FILE + compressionExtension(params.compressionType));
  outputFileName = Directory::mixNameComponents(params.indexPath, TMP_FILE_NAME + compressionExtension(params.compressionType));
  logMsg(LOG_INFO, "Patching \'%s\' to \'%s\'", inputFileName.c_str(), outputFileName.c_str());
  m_listener.onPatchingFile(inputFileName);
  reader = createRebuildReader(inputFileName, params);
  writer = createRebuildWriter(outputFileName, params);
  reader->init();
  while(reader->readNext(sect))
    {
      const std::string fileName = PkgSection::getPkgFileName(sect);
      if(fileName.empty())
	throw IndexCoreException(IndexCoreException::InternalIOProblem);
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
  File::unlink(inputFileName);
  File::move(outputFileName, inputFileName);

    //Sources descriptions file;
  for(BoolVector::size_type i = 0;i < skipToAdd.size();i++)
    skipToAdd[i] = 0;
  inputFileName = Directory::mixNameComponents(params.indexPath, REPO_INDEX_SOURCES_DESCR_FILE + compressionExtension(params.compressionType));
  outputFileName = Directory::mixNameComponents(params.indexPath, TMP_FILE_NAME + compressionExtension(params.compressionType));
  logMsg(LOG_INFO, "Patching \'%s\' to \'%s\'", inputFileName.c_str(), outputFileName.c_str());
  m_listener.onPatchingFile(inputFileName);
  reader = createRebuildReader(inputFileName, params);
  writer = createRebuildWriter(outputFileName, params);
  reader->init();
  while(reader->readNext(sect))
    {
      const std::string fileName = PkgSection::getPkgFileName(sect);
      if(fileName.empty());
      throw IndexCoreException(IndexCoreException::InternalIOProblem);
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
	  writer->writeData(PkgSection::saveDescr(pkgs[i], params.changeLogSources));
	  logMsg(LOG_DEBUG, "File \'%s\' added to index", pkgs[i].fileName.c_str());
	}
  reader->close();
  writer->close();
  File::unlink(inputFileName);
  File::move(outputFileName, inputFileName);

    //Complete packages file;
  for(BoolVector::size_type i = 0;i < skipToAdd.size();i++)
    skipToAdd[i] = 0;
 inputFileName = Directory::mixNameComponents(params.indexPath, REPO_INDEX_PACKAGES_COMPLETE_FILE);
  outputFileName = Directory::mixNameComponents(params.indexPath, TMP_FILE_NAME);
  logMsg(LOG_INFO, "Patching \'%s\' to \'%s\'", inputFileName.c_str(), outputFileName.c_str());
  m_listener.onPatchingFile(inputFileName);
  reader = createRebuildReaderNoCompression(inputFileName);
  writer = std::auto_ptr<UnifiedOutput>(new StdOutput(outputFileName));
  reader->init();
  while(reader->readNext(sect))
    {
      const std::string fileName = PkgSection::getPkgFileName(sect);
      if(fileName.empty())
	throw IndexCoreException(IndexCoreException::InternalIOProblem);
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
  File::unlink(inputFileName);
  File::move(outputFileName, inputFileName);

    //File list;
  for(BoolVector::size_type i = 0;i < skipToAdd.size();i++)
    skipToAdd[i] = 0;
  inputFileName = Directory::mixNameComponents(params.indexPath, REPO_INDEX_PACKAGES_FILELIST_FILE + compressionExtension(params.compressionType));
  outputFileName = Directory::mixNameComponents(params.indexPath, TMP_FILE_NAME + compressionExtension(params.compressionType));
  logMsg(LOG_INFO, "Patching \'%s\' to \'%s\'", inputFileName.c_str(), outputFileName.c_str());
  m_listener.onPatchingFile(inputFileName);
  reader = createRebuildReader(inputFileName, params);
  writer = createRebuildWriter(outputFileName, params);
  reader->init();
  while(reader->readNext(sect))
    {
      const std::string fileName = PkgSection::getPkgFileName(sect);
      if(fileName.empty())
	throw IndexCoreException(IndexCoreException::InternalIOProblem);
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
	  writer->writeData(PkgSection::saveFileList(pkgs[i]));
	  logMsg(LOG_DEBUG, "File \'%s\' added to index", pkgs[i].fileName.c_str());
	}
  reader->close();
  writer->close();
  File::unlink(inputFileName);
  File::move(outputFileName, inputFileName);

  logMsg(LOG_INFO, "Updating md5sums");
  m_listener.onChecksumWriting();
  StringVector files;
  for(Md5File::ItemVector::size_type i = 0;i < md5File.items.size();i++)
    if (md5File.items[i].fileName != REPO_INDEX_INFO_FILE)
      files.push_back(md5File.items[i].fileName);
  for(StringVector::size_type i = 0;i < files.size();i++)
    md5File.removeItem(files[i]);
  for(StringVector::size_type i = 0;i < files.size();i++)
    md5File.addItemFromFile(files[i], Directory::mixNameComponents(params.indexPath, files[i]));
  md5File.saveToFile(Directory::mixNameComponents(params.indexPath, params.md5sumFileName));
  logMsg(LOG_DEBUG, "Repository index in \'%s\' fixing completed successfully", params.indexPath.c_str());
}

void IndexCore::refilterProvides(const RepoParams& params)
{
  assert(!params.indexPath.empty());
  logMsg(LOG_DEBUG, "Starting reference fixing in \'%s\'", params.indexPath.c_str());
  m_listener.onChecksumVerifying();
  if (params.md5sumFileName.empty())
    throw IndexCoreException(IndexCoreException::MissedChecksumFileName);
  Md5File md5File;
  md5File.loadFromFile(Directory::mixNameComponents(params.indexPath, params.md5sumFileName));
  for(Md5File::ItemVector::size_type i = 0;i < md5File.items.size();i++)
    {
      logMsg(LOG_DEBUG, "Verifying checksum for \'%s\'", Directory::mixNameComponents(params.indexPath, md5File.items[i].fileName).c_str());
      if (!md5File.verifyItem(i, Directory::mixNameComponents(params.indexPath, md5File.items[i].fileName)))
	throw IndexCoreException(IndexCoreException::CorruptedFile, Directory::mixNameComponents(params.indexPath, md5File.items[i].fileName));
    }
  const std::string pkgFileName = Directory::mixNameComponents(params.indexPath, REPO_INDEX_PACKAGES_FILE + compressionExtension(params.compressionType));
  const std::string pkgCompleteFileName = Directory::mixNameComponents(params.indexPath, REPO_INDEX_PACKAGES_COMPLETE_FILE);
  logMsg(LOG_DEBUG, "pkgFileName=\'%s\', pkgCompleteFileName=\'%s\'", pkgFileName.c_str(), pkgCompleteFileName.c_str());
  StringSet internalReferences, externalReferences;
  if (params.filterProvidesByRefs)
    {
      logMsg(LOG_DEBUG, "Provides filtering by references is enabled, we have %zu external references sources", params.providesRefsSources.size());
      for(StringVector::size_type i = 0;i < params.providesRefsSources.size();i++)
	{
	  logMsg(LOG_DEBUG, "Collecting external references in \'%s\'", params.providesRefsSources[i].c_str());
	  m_listener.onReferenceCollecting(params.providesRefsSources[i]);
	  collectRefs(params.providesRefsSources[i], externalReferences);
	}
      logMsg(LOG_DEBUG, "%zu external references collected", externalReferences.size());
      logMsg(LOG_DEBUG, "Reading \'%s\' to pick up internal references", pkgFileName.c_str());
	  m_listener.onReferenceCollecting(params.indexPath);
      std::auto_ptr<AbstractTextFormatSectionReader> reader = createRebuildReader(pkgFileName, params);
      size_t count = 0;
      std::string sect;
      reader->init();
      while(reader->readNext(sect))
	{
	PkgSection::extractProvidesReferences(sect, internalReferences);
	count++;
	}
      reader->close();
      logMsg(LOG_DEBUG, "%zu internal references collected in %zu package records", internalReferences.size(), count);
    }
  logMsg(LOG_DEBUG, "Performing unlink() for \'%s\'", pkgFileName.c_str());
  File::unlink(pkgFileName);
  m_listener.onProvidesCleaning();
  logMsg(LOG_DEBUG, "Creating new \'%s\'", pkgFileName.c_str());
  std::auto_ptr<UnifiedOutput> pkgFile = createRebuildWriter(pkgFileName, params);
  std::ifstream is(pkgCompleteFileName.c_str());
  if (!is.is_open())
    throw IndexCoreException(IndexCoreException::InternalIOProblem);
  while(1)
    {
      std::string line;
      std::getline(is, line);
      if (!is)
	break;
      std::string pkgName;
      if (!PkgSection::isProvidesLine(line, pkgName))
	{
	  pkgFile->writeData(line + "\n");
	  continue;
	}
      if (params.filterProvidesByRefs)
	{
	  if (internalReferences.find(pkgName) != internalReferences.end() || 
	      externalReferences.find(pkgName) != externalReferences.end() ||
	      (!params.filterProvidesByDirs.empty() && fileFromDirs(pkgName, params.filterProvidesByDirs)))
	    pkgFile->writeData(line + "\n");
	} else
	{
	  if (params.filterProvidesByDirs.empty() || fileFromDirs(pkgName, params.filterProvidesByDirs))
	    pkgFile->writeData(line + "\n");
	}
    }
  pkgFile->close();
  logMsg(LOG_DEBUG, "New packages file writing completed");
  m_listener.onChecksumWriting();
  logMsg(LOG_DEBUG, "Updating md5sum for \'%s\' and writing new file", pkgFileName.c_str());
  md5File.removeItem(File::baseName(pkgFileName));
  md5File.addItemFromFile(File::baseName(pkgFileName), pkgFileName);
  md5File.saveToFile(Directory::mixNameComponents(params.indexPath, REPO_INDEX_MD5SUM_FILE));
  logMsg(LOG_DEBUG, "Exiting references fixing procedure, everything done successfully");
}

void IndexCore::collectRefs(const std::string& dirName, StringSet& res) 
{
  logMsg(LOG_INFO, "Collecting references in \'%s\'", dirName.c_str());
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
      logMsg(LOG_INFO, "Directory \'%s\' does not contain a valid repo index:%s error:%s", dirName.c_str(), e.getType().c_str(), e.getMessage().c_str());
    }
  logMsg(LOG_INFO, "Since repository index reading failed  we are looking for just packages itself");
  std::auto_ptr<AbstractPackageBackEnd> backend = CREATE_PACKAGE_BACKEND;
  std::auto_ptr<Directory::Iterator> it = Directory::enumerate(dirName);
  while(it->moveNext())
    {
      if (it->name() == "." || it->name() == "..")
	continue;
      if (!backend->validPkgFileName(it->name()))
	continue;
      PkgFile pkgFile;
      backend->readPackageFile(it->fullPath(), pkgFile);
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

std::auto_ptr<AbstractTextFormatSectionReader> createRebuildReaderNoCompression(const std::string& fileName)
{
  std::auto_ptr<TextFormatSectionReader> reader(new TextFormatSectionReader());
  reader->open(fileName);
  return std::auto_ptr<AbstractTextFormatSectionReader>(reader.release());
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
