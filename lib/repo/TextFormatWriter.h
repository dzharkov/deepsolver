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

#ifndef DEEPSOLVER_TEXT_FORMAT_WRITER_H
#define DEEPSOLVER_TEXT_FORMAT_WRITER_H

#include"Pkg.h"
#include"IndexCore.h"
#include"AbstractConsoleMessages.h"
#include"utils/TextFiles.h"

class AbstractRequireFilter
{
public:
  AbstractRequireFilter() {}
  virtual ~AbstractRequireFilter() {}

public:
  virtual bool excludeRequire(const std::string& requireEntry) const = 0;
}; //class abstractRequireFilter;

/**\brief The class to write repo index data in text format
 *
 * This is the main class for writing repository index data in text
 * format. It must be used by IndexCore class only and not purposed for
 * direct using by library client applications. By the way it performs
 * provides filtering with the options used during the invocation and
 * saves provide resolving data. To be memory-efficient this class
 * creates and then removes one or more temporary files. It means that
 * directory for repository index must have more free disk space than
 * just for index data itself.
 *
 * The work consists of three steps: initialization, package data
 * collecting and commit. On initialization only temporary file for
 * package data storing is created . On every add operation during the
 * collecting step all required information about single package file is
 * written to this temporary file. Main work is done on commit step. This
 * step makes provides filtering and building provide resolving table. 
 *
 * Provide resolving table shows what packages can be used for every
 * provide entry dependency. Building this information during repository
 * index creation can significantly reduce time used for package
 * installation and removing operations. 
 *
 * There are two options for provide filtering:register the provide entry
 * if corresponding require/conflict reference is present and filter provides by
 * directories list. The second case is actual only for provides as files
 * from the file list of the package. The filtering mode by require/conflict
 * references includes dependencies from collected packages and in
 * additional from optional string list.
 *
 * \sa IndexCore TextFormatReader InfoFileWriter InfoFileReader
 */
class TextFormatWriter
{
public:
  TextFormatWriter(const AbstractRequireFilter& requireFilter,
			    const RepoIndexParams& params,
			    AbstractConsoleMessages& console,
			    const std::string& dir,
			    const StringSet& additionalRefs);

  virtual ~TextFormatWriter() {}

public:
  void initBinary();
  void initSource();

  void addBinary(const PkgFile& pkgFile, const StringList& fileList);
  void addSource(const PkgFile& pkgFile);

  void commitBinary();
  void commitSource();

  std::string getRpmsFileName() const
  {
    return m_rpmsFileName;
  }

  std::string getSrpmsFileName() const
  {
    return m_srpmsFileName;
  }

  std::string getProvidesFileName() const
  {
    return m_providesFileName;
  }

private:
  void firstProvideReg(const std::string& pkgName, const std::string& provideName);
  void secondProvideReg(const std::string& pkgName, const std::string& provideName);
  void secondPhase();
  void additionalPhase();

private:
  const AbstractRequireFilter& m_requireFilter;
  const RepoIndexParams& m_params;
  AbstractConsoleMessages& m_console;
  const std::string m_dir;
  const std::string m_rpmsFileName, m_srpmsFileName, m_providesFileName;
  std::string m_tmpFileName;//Additional phase modifies this file name;
  std::auto_ptr<AbstractTextFileWriter> m_tmpFile, m_srpmsFile;
  const bool m_filterProvidesByRefs;
  const StringSet& m_additionalRefs;
  const StringList& m_filterProvidesByDirs;
  StringSet m_refsSet;
}; //class TextFormatWriter;

#endif //DEEPSOLVER_TEXT_FORMAT_WRITER_H;
