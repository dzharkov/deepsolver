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
kaka

    }




  for(StringVector::size_type i = 0;i < params.pkgSources.size();i++)
    {

    }
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
  TextFormatWriter handler(requireFilter, params, indexDir, additionalRefs);
  //Binary packages;
  handler.initBinary();
  logMsg(LOG_DEBUG, "RepoIndexTextFormatWriter created and initialized for binary packages");
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
  handler.commitBinary();
  logMsg(LOG_DEBUG, "Committed %zu binary packages", count);
  //Source packages;
  logMsg(LOG_DEBUG, "Binary packages processing is finished, switching to sources");
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
  logMsg(LOG_DEBUG, "Committed %zu source packages", count);
  handler.commitSource();
  //Saving md5sum;
  std::auto_ptr<AbstractTextFileWriter> md5sum = createTextFileWriter(TextFileStd, Directory::mixNameComponents(indexDir, REPO_INDEX_MD5SUM_FILE));
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
}

std::string IndexCore::compressionExtension(char compressionType)
{
  if (compressionType == RepoParams::CompressionTypeGzip)
    return COMPRESSION_SUFFIX_GZIP;
  assert(compressionType == RepoIParams::CompressionTypeNone);
  return "";
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

/*
  if (m_filterProvidesByRefs)//It means the second phase is needed;
    m_packagesFile = createTextFileWriter(TextFileStd, m_tmpFileName); else
    m_packagesFile = createTextFileWriter(mapTextFileType(m_params.compressionType), m_packagesFileName);
    m_packagesDescrFile = createTextFileWriter(mapTextFileType(m_params.compressionType), m_packagesDescrFileName);
  m_sourcesFile = createTextFileWriter(mapTextFileType(m_params.compressionType), m_sourcesFileName);
  m_sourcesDescrFile = createTextFileWriter(mapTextFileType(m_params.compressionType), m_sourcesDescrFileName);
  m_packagesFile->close();
  m_packagesDescrFile->close();
  if (m_filterProvidesByRefs)
    secondPhase();
  m_sourcesFile->close();
  m_sourcesDescrFile->close();
*/

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

