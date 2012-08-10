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
#include"rpm/RpmFile.h"
#include"rpm/RpmFileHeaderReader.h"
#include"utils/MD5.h"

#define TMP_FILE_NAME "tmp_packages_data"

class UnifiedOutput
{
public:
  UnifiedOutput() {}
  virtual ~UnifiedOutput() 
  {
    close();
  }

public:
  virtual void writeData(const std:;string& str = 0;)
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

  virtual ~StdOutput() {}

public:
  void open(const std::string& fileName)
  {
    assert(!m_stream.is_opened());
    assert(!fileName.empty());
    m_stream.open(fileName.c_str());
    assert(m_stream.is_opened());//FIXME:Must be an exception;
  }

  void writeData(const std::string& str)
  {
    assert(m_stream.is_opened());
    m_stream << str;
  }

  void close()
  {
    if (!m_stream.is_opened())
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

  virtual ~GzipOutput() {}

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

void IndexCore::buildRepo(const RepoIndexParams& params)
{
  assert(!params.indexPath.empty());
  assert(!params.pkgSources.empty());
  Directory::ensureExists(params.indexPath);
  params.writeInfoFile(Directory::mixNameComponents(params.indexPath, REPO_INDEX_INFO_FILE));
  StringSet providesRefs;
  for(StringVector::size_type i = 0;i < providesRefsSources.size();i++)
    collectRefs(providesRefsSources[i], providesRefs);

  const std::string pkgFileName = REPO_INDEX_PACKAGES_FILE + compressionExtension();
  const std::string pkgDescrFileName = REPO_INDEX_PACKAGES_DESCR_FILE + compressionExtension();
  const std::string srcFileName = REPO_INDEX_SOURCES_FILE + compressionExtension();
  const std::string srcDescrFileName = REPO_INDEX_SOURCES_DESCR_FILE + compressionExtension();
  const std::string tmpFileName = TMP_FILE_NAME;
  std::auto_ptr<UnifiedOutput> pkgFile, pkgDescrFile, srcFile, srcDescrFile;
  if (params.compressionType == RepoParams.CompressionTypeGzip)
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

  std::auto_ptr<AbstractPackageBackEnd> backend = createRpmBackEnd();//Or anything else user wants;
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
	  PkgFile pkgFile;
	  backend->readPackageFile(it->getFullPath(), pkgFile);
	  pkgFile.fileName = it->getFileName();
	  if (!pkgFIle.isSource)
	    {
	      pkgFile->write(pkgSection::saveBaseInfo(pkgFile));
	      pkgDescrFile->write(pkgSection::saveBaseInfo(pkgFile));
	    } else
	    {
	      srcFile->write(pkgSection::saveBaseInfo(pkgFile));
	      srcDescrFile->write(pkgSection::saveBaseInfo(pkgFile));
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
	pkgFile = std::auto_ptr<UnifiedOutput>(GzipOutput(pkgFileName)); else
	pkgFile = std::auto_ptr<UnifiedOutput>(StdOutput(pkgFileName));
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
	  pkgFile->write(sect);
	} //while(packages);
      reader.close();
      pkgFile->close();
  File::unlink(tmpFileName);
    } //Additional phase;
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
}

std::string IndexCore::compressionExtension(char compressionType)
{
  if (compressionType == RepoParams::CompressionTypeGzip)
    return COMPRESSION_SUFFIX_GZIP;
  assert(compressionType == RepoIParams::CompressionTypeNone);
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
