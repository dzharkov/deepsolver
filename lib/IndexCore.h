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

#ifndef DEEPSOLVER_INDEX_CORE_H
#define DEEPSOLVER_INDEX_CORE_H

#include"repo/RepoParams.h"

class IndexCoreException: public DeepsolverException 
{
public:
  IndexCoreException() {}
  IndexCoreException(const std::string& message):
    m_message(message) {}

  virtual ~IndexCoreException() {}

public:
  std::string getType() const
  {
    return "index core";
  }

  std::string getMessage() const
  {
    return m_message;
  }

private:
  const std::string m_message;
}; //class IndexCoreException;

class IndexCore
{
public:
  IndexCore() {}

public:
  void build(const RepoIndexParams& params);

private:
  void collectRefs(const std::string& dirName, StringSet& res);
  void collectRefsFromDirs(const StringList& dirs, StringSet& res);
  void processPackages(const std::string& indexDir, const std::string& rpmsDir, const std::string& srpmsDir, const RepoIndexParams& params);
  void writeInfoFile(const std::string& fileName, const RepoIndexParams& params);

 private:
  AbstractIndexConstructionListener& m_listener;
}; //class IndexCore;

/*
private:
  void secondPhase();

private:
  static int mapTextFileType(int compressionType);
  static std::string addCompressionExtension(const std::string& fileName, const RepoIndexParams& params);
  static bool fileFromDirs(const std::string& fileName, const StringList& dirs);

private:
  AbstractIndexConstructionListener& m_listener;

private://Files;
  const AbstractRequireFilter& m_requireFilter;
  const RepoIndexParams& m_params;
  const std::string m_dir;
  const std::string m_tmpFileName, m_packagesFileName, m_packagesDescrFileName, m_sourcesFileName, m_sourcesDescrFileName;
  std::auto_ptr<AbstractTextFileWriter> m_packagesFile, m_packagesDescrFile, m_sourcesFile, m_sourcesDescrFile;

private://Provides filtering;
  const bool m_filterProvidesByRefs;
  const StringSet& m_additionalRefs;
  StringSet m_collectedRefs;
  const StringList& m_filterProvidesByDirs;
}; //class TextFormatWriter;

*/

#endif //DEEPSOLVER_INDEX_CORE_H;
