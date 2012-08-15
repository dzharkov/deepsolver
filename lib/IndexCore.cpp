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

#define TMP_FILE_NAME "tmp_packages_data"

class UnifiedOutput
{
public:
  UnifiedOutput() {}
  virtual ~UnifiedOutput() {}

public:
  virtual void writeData(const std::string& str) = 0;
  virtual void close();
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

void IndexCore::buildIndex(const RepoParams& params)
{
  assert(!params.indexPath.empty());
  assert(!params.pkgSources.empty());
  Directory::ensureExists(params.indexPath);
  params.writeInfoFile(Directory::mixNameComponents(params.indexPath, REPO_INDEX_INFO_FILE));
  StringSet providesRefs;
  for(StringVector::size_type i = 0;i < params.providesRefsSources.size();i++)
    collectRefs(params.providesRefsSources[i], providesRefs);

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
	  if (!pkg.isSource)
	    {
	      pkgFile->writeData(PkgSection::saveBaseInfo(pkg));
	      pkgDescrFile->writeData(PkgSection::saveBaseInfo(pkg));
	    } else
	    {
	      srcFile->writeData(PkgSection::saveBaseInfo(pkg));
	      srcDescrFile->writeData(PkgSection::saveBaseInfo(pkg));
	    }
	} //for package files;
    } //for listed directories;
  pkgFile->close();
  pkgDescrFile->close();
  srcFile->close();
  srcDescrFile->close();
  //Additional phase for provides filtering if needed;
  if (params.filterProvidesByRefs)
    {
      TextFormatSectionReader reader;
      reader.open(tmpFileName);
      if (params.compressionType == RepoParams::CompressionTypeGzip)
	pkgFile = std::auto_ptr<UnifiedOutput>(new GzipOutput(pkgFileName)); else
	pkgFile = std::auto_ptr<UnifiedOutput>(new StdOutput(pkgFileName));
      std::string sect;
      while(reader.readNext(sect))
	{
	  /*
      const std::string provideName = getPkgRelName(tail);
      assert(!provideName.empty());
      if (m_collectedRefs.find(provideName) != m_collectedRefs.end() || 
	  m_additionalRefs.find(provideName) != m_additionalRefs.end() ||
	  (!m_filterProvidesByDirs.empty() && fileFromDirs(provideName, m_filterProvidesByDirs)))
	  */
	  //FIXME:filtering;
	  pkgFile->writeData(sect);
	} //while(packages);
      reader.close();
      pkgFile->close();
  File::unlink(tmpFileName);
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

bool IndexCore::fileFromDirs(const std::string& fileName, const StringList& dirs)
{
  std::string tail;
  for(StringList::const_iterator it = dirs.begin();it != dirs.end();it++)
    {
      if (stringBegins(fileName, *it, tail))
	return 1;
    }
  return 0;
}
