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
#include"AbstractIndexConstructionListener.h"
#include"utils/TextFiles.h"

class AbstractRequireFilter
{
public:
  AbstractRequireFilter() {}
  virtual ~AbstractRequireFilter() {}

public:
  virtual bool excludeRequire(const std::string& requireEntry) const = 0;
}; //class abstractRequireFilter;

/**\brief Write repository index data in text format
 *
 * This is the main class for writing repository index data in text
 * format. It is designed to be used by IndexCore class only and not purposed for
 * direct using by library client. By the way it performs
 * provides filtering with various options. To be memory-efficient this class
 * creates and then removes one or more temporary files. That means that
 * directory for repository index must have more free disk space than
 * just for index data itself.
 *
 * The work consists of three steps: initialization, package data
 * collecting and commit. Initialization step only creates temporary file
 * for package data storing but nothing writes. On every add operation during the
 * collecting step all required information about each package
 * goes into temporary file. Main work is done on commit step. This
 * step makes provides filtering and writes the final index files.
 *
 * There are two options for provide filtering:register the provide entry
 * if corresponding require/conflict reference is present and filter provides by
 * directories list. The second case is actual only for provides as files
 * from the file list of the package. The filtering mode by require/conflict
 * references includes dependencies from collected packages and in
 * addition from optional string list.
 *
 * \sa IndexCore TextFormatReader InfoFileWriter InfoFileReader AbstractTextFormatWriterListener
 */
class TextFormatWriter
{
public:
  /**\brief The constructor
   *
   * \param [in] listener The reference to the handling object for user status notification
   * \param [in] requireFilter The object for excluding needless requires
   * \param [in] params The various repository index parameters
   * \param [in] dir The directory to put newly created index in
   * \param [in] additionalRefs The set of package names to use as references in provide filtering
   */
  TextFormatWriter(AbstractIndexConstructionListener& listener,
		   const AbstractRequireFilter& requireFilter,
		   const RepoIndexParams& params,
		   const std::string& dir,
		   const StringSet& additionalRefs);

  /**\brief The destructor*/
  virtual ~TextFormatWriter() {}

public:
  void initBinary();
  void initSource();

  void addBinary(const PkgFile& pkgFile, const StringList& fileList);
  void addSource(const PkgFile& pkgFile);

  void commitBinary();
  void commitSource();

  std::string getPackagesFileName() const
  {
    return m_packagesFileName;
  }

  std::string getSourcesFileName() const
  {
    return m_sourcesFileName;
  }

private:
  void secondPhase();
  void additionalPhase();

private:
  static int mapTextFileType(int compressionType);
  static std::string addCompressionExtension(const std::string& fileName, const RepoIndexParams& params);
  static std::string getPkgRelName(const std::string& line);
  static std::string saveNamedPkgRel(const NamedPkgRel& r);
  static std::string saveFileName(const std::string& fileName);
  static std::string encodeMultiline(const std::string& s);
  static std::string encodeChangeLogEntry(const ChangeLogEntry& entry);
  static bool fileFromDirs(const std::string& fileName, const StringList& dirs);

private:
  AbstractIndexConstructionListener& m_listener;
  const AbstractRequireFilter& m_requireFilter;
  const RepoIndexParams& m_params;
  const std::string m_dir;
  const std::string m_packagesFileName, m_sourcesFileName;
  std::string m_tmpFileName;//Additional phase modifies this file name;
  std::auto_ptr<AbstractTextFileWriter> m_tmpFile, m_sourcesFile;
  const bool m_filterProvidesByRefs;
  const StringSet& m_additionalRefs;
  const StringList& m_filterProvidesByDirs;
  StringSet m_refsSet;
}; //class TextFormatWriter;

#endif //DEEPSOLVER_TEXT_FORMAT_WRITER_H;
