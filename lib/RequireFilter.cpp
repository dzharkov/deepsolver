
#include"deepsolver.h"
#include"RequireFilter.h"
#include"TextFiles.h"

void RequireFilter::load(const std::string& fileName)
{
  std::auto_ptr<AbstractTextFileReader> reader = createTextFileReader(TextFileStd, fileName);
  std::string line;
  while(reader->readLine(line))
    {
      std::string::size_type k = 0;
      while(k < line.length() && BLANK_CHAR(line[k]))
	k++;
      if (k >= line.length())
	continue;
      if (line[k] == '#')
	continue;
      m_requiresToExclude.push_back(trim(line));
    }
}

bool RequireFilter::excludeRequire(const std::string& requireEntry) const
{
  for(StringVector::size_type i = 0;i < m_requiresToExclude.size();i++)
    if (m_requiresToExclude[i] == requireEntry)
      return 1;
  return 0;
}
