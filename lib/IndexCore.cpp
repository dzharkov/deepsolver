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
#include"utils/MD5.h"
#include"utils/GZipInterface.h"

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
  GZipOutputFile m_file;
}; //class GzipOutput;

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

void IndexCore::buildIndex(const RepoParams& params)
{
  assert(!params.indexPath.empty());
  assert(!params.pkgSources.empty());
  Directory::ensureExists(params.indexPath);
  logMsg(LOG_DEBUG, "Starting index creation in \'%s\'", params.indexPath.c_str());
  params.writeInfoFile(Directory::mixNameComponents(params.indexPath, REPO_INDEX_INFO_FILE));
  StringSet internalProvidesRefs, externalProvidesRefs;
  for(StringVector::size_type i = 0;i < params.providesRefsSources.size();i++)
    {
      m_listener.onReferenceCollecting(params.providesRefsSources[i]);
      collectRefs(params.providesRefsSources[i], externalProvidesRefs);
    }
  logMsg(LOG_DEBUG, "Has %zu external provides references", externalProvidesRefs.size());
  const std::string pkgFileName = REPO_INDEX_PACKAGES_FILE + compressionExtension(params.compressionType);
  const std::string pkgDescrFileName = REPO_INDEX_PACKAGES_DESCR_FILE + compressionExtension(params.compressionType);
  const std::string srcFileName = REPO_INDEX_SOURCES_FILE + compressionExtension(params.compressionType);
  const std::string srcDescrFileName = REPO_INDEX_SOURCES_DESCR_FILE + compressionExtension(params.compressionType);
  const std::string tmpFileName = TMP_FILE_NAME;
  std::auto_ptr<UnifiedOutput> pkgFile, pkgDescrFile, srcFile, srcDescrFile;
  if (params.compressionType == RepoParams::CompressionTypeGzip)
    {
      if (params.filterProvidesByRefs)//That means additional phase is required;
	pkgFile = std::auto_ptr<UnifiedOutput>(new StdOutput(tmpFileName)); else
	pkgFile = std::auto_ptr<UnifiedOutput>(new GzipOutput(pkgFileName));
      pkgDescrFile = std::auto_ptr<UnifiedOutput>(new GzipOutput(pkgDescrFileName));
      srcFile = std::auto_ptr<UnifiedOutput>(new GzipOutput(srcFileName));
      srcDescrFile = std::auto_ptr<UnifiedOutput>(new GzipOutput(srcDescrFileName));
    } else 
    {
      assert(params.compressionType == RepoParams::CompressionTypeNone);
      if (params.filterProvidesByRefs)//That means additional phase is required;
	pkgFile = std::auto_ptr<UnifiedOutput>(new StdOutput(tmpFileName)); else
	pkgFile = std::auto_ptr<UnifiedOutput>(new StdOutput(pkgFileName));
      pkgDescrFile = std::auto_ptr<UnifiedOutput>(new StdOutput(pkgDescrFileName));
      srcFile = std::auto_ptr<UnifiedOutput>(new StdOutput(srcFileName));
      srcDescrFile = std::auto_ptr<UnifiedOutput>(new StdOutput(srcDescrFileName));
    }
  std::auto_ptr<AbstractPackageBackEnd> backend = CREATE_PACKAGE_BACKEND;
  //We ready to collect information about packages in specified directories;
  for(StringVector::size_type i = 0;i < params.pkgSources.size();i++)
    {
      logMsg(LOG_DEBUG, "Reading packages in \'%s\'", params.pkgSources[i].c_str());
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
	  for(NamedPkgRelVector::size_type k = 0;k < pkg.requires.size();k++)
	    internalProvidesRefs.insert(pkg.requires[k].pkgName);
	  for(NamedPkgRelVector::size_type k = 0;k < pkg.conflicts.size();k++)
	    internalProvidesRefs.insert(pkg.conflicts[k].pkgName);
	  if (!pkg.isSource)
	    {
	      pkgFile->writeData(PkgSection::saveBaseInfo(pkg, params.filterProvidesByRefs?StringVector():params.filterProvidesByDirs));
	      pkgDescrFile->writeData(PkgSection::saveDescr(pkg, params.changeLogBinary));
	    } else
	    {
	      srcFile->writeData(PkgSection::saveBaseInfo(pkg, params.filterProvidesByRefs?StringVector():params.filterProvidesByDirs));
	      srcDescrFile->writeData(PkgSection::saveDescr(pkg, params.changeLogSources));
	    }
	} //for package files;
    } //for listed directories;
  pkgFile->close();
  pkgDescrFile->close();
  srcFile->close();
  srcDescrFile->close();
  logMsg(LOG_DEBUG, "Has %zu internal provides references and %zu external provides references", internalProvidesRefs.size(), externalProvidesRefs.size());
  //Additional phase for provides filtering if needed;
  if (params.filterProvidesByRefs)
    {
      m_listener.onProvidesCleaning();
      std::ifstream is(tmpFileName.c_str());
      if (!is.is_open())
	throw IndexCoreException(IndexErrorInternalIOProblem);
      if (params.compressionType == RepoParams::CompressionTypeGzip)
	pkgFile = std::auto_ptr<UnifiedOutput>(new GzipOutput(pkgFileName)); else
	pkgFile = std::auto_ptr<UnifiedOutput>(new StdOutput(pkgFileName));
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
      //FIXME:  File::unlink(tmpFileName);
    } //Additional phase;
  /*
  MD5 md5;
  md5.init();
  md5.updateFromFile(Directory::mixNameComponents(indexDir, REPO_INDEX_INFO_FILE));
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
  */
}

void IndexCore::rebuildIndex(const RepoParams& params, const StringVector& toAdd, const StringVector& toRemove)
{
  logMsg(LOG_DEBUG, "starting index patching process with %zu items to add and %zu items to remove", toAdd.size(), toRemove.size());
  std::string sect;
  std::string inputFileName, outputFileName;
    std::auto_ptr<AbstractTextFormatSectionReader> reader;
  std::auto_ptr<UnifiedOutput> writer;

    //Packages file;
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
      StringVector::size_type i = 0;
      for(i = 0;i < toRemove.size();i++)
	if (fileName == toRemove[i])
	  break;
      if (i >= toRemove.size())
	writer->writeData(sect); else
	logMsg(LOG_DEBUG, "Found package to exclude: \'%s\'", fileName.c_str());
    }
  reader->close();
  writer->close();

    //Packages descriptions file;
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
      StringVector::size_type i = 0;
      for(i = 0;i < toRemove.size();i++)
	if (fileName == toRemove[i])
	  break;
      if (i >= toRemove.size())
	writer->writeData(sect); else
	logMsg(LOG_DEBUG, "Found package to exclude: \'%s\'", fileName.c_str());
    }
  reader->close();
  writer->close();

    //Sources file;
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
      StringVector::size_type i = 0;
      for(i = 0;i < toRemove.size();i++)
	if (fileName == toRemove[i])
	  break;
      if (i >= toRemove.size())
	writer->writeData(sect); else
	logMsg(LOG_DEBUG, "Found package to exclude: \'%s\'", fileName.c_str());
    }
  reader->close();
  writer->close();

    //Sources descriptions file;
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
      StringVector::size_type i = 0;
      for(i = 0;i < toRemove.size();i++)
	if (fileName == toRemove[i])
	  break;
      if (i >= toRemove.size())
	writer->writeData(sect); else
	logMsg(LOG_DEBUG, "Found package to exclude: \'%s\'", fileName.c_str());
    }
  reader->close();
  writer->close();

}

void IndexCore::collectRefs(const std::string& dirName, StringSet& res) 
{
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

std::string IndexCore::compressionExtension(char compressionType)
{
  if (compressionType == RepoParams::CompressionTypeGzip)
    return COMPRESSION_SUFFIX_GZIP;
  assert(compressionType == RepoParams::CompressionTypeNone);
  return "";
}

bool IndexCore::fileFromDirs(const std::string& fileName, const StringVector& dirs)
{
  std::string tail;
  for(StringVector::const_iterator it = dirs.begin();it != dirs.end();it++)
    if (stringBegins(fileName, *it, tail))
      return 1;
  return 0;
}
