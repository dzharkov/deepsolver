
#include"basic-header.h"
#include"RepoIndexTextFormat.h"
#include"IndexCoreException.h"

#define TMP_FILE "tmp_file1"

void RepoIndexTextFormat::init()
{
  const std::string tmpFileName = concatUnixPath(m_dir, TMP_FILE);
  m_os.open(tmpFileName.c_str());
  if (!m_os)
    INDEX_CORE_STOP("Error creating temporary file \'" + tmpFileName + "\'");
}

void RepoIndexTextFormat::add(const PkgFile& pkgFile,
			      const NamedPkgRelList& provides,
			      const NamedPkgRelList& requires,
			      const NamedPkgRelList& conflicts,
			      const NamedPkgRelList& obsoletes,
			      const StringList& fileList)
{
  m_os << "[" << File::baseName(pkgFile.fileName) << "]" << std::endl;
  m_os << "name=" << pkgFile.name << std::endl;
  m_os << "version=" << pkgFile.version << std::endl;
  for(NamedPkgRelList::const_iterator it = provides.begin();it != provides.end();it++)
    {
      m_os << "provides:" << (*it) << std::endl;
      firstProvideReg(pkgFile.name, it->pkgName);
    }
    m_os << std::endl;
}

void RepoIndexTextFormat::commit()
{
  m_os.close();
  //FIXME:
}

void RepoIndexTextFormat::firstProvideReg(const std::string& pkgName, const std::string& provideName)
{
  StringToIntMap::iterator it;
  it = m_provideMap.find(pkgName);
  if (it == m_provideMap.end())
    m_provideMap.insert(StringToIntMap::value_type(pkgName, 0));
  it = m_provideMap.find(provideName);
  if (it == m_provideMap.end())
    m_provideMap.insert(StringToIntMap::value_type(provideName, 1)); else
    it->second++;
}
