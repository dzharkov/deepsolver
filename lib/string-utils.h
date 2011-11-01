
#ifndef FIXME_STRING_UTILS_H
#define FIXME_STRING_UTILS_H

bool hasNonSpaces(const std::string& s)
{
  //FIXME:BLANK_CHARS();
  for(std::string::size_type i = 0;i < s.length();i++)
    if (s[i] != ' ')
return 1;
return 0;
}

std::string concatUnixPath(const std::string& part1, const std::string& part2)
{
  if (part1.empty() && part2.empty())
    return std::string();
  if (part1.empty())
    return part2;
  if (part2.empty())
    return part1;
  const std::string::value_type lastChar1 = part1[part1.length() - 1], firstChar2 = part2[0];
  if (lastChar1 != '/' && firstChar2 != '/')
    return part1 + "/" + part2;
  if ((lastChar1 == '/' && firstChar2 != '/') || (lastChar1 != '/' && firstChar2 == '/'))
    return part1 + part2;
  assert(lastChar1 == '/' && firstChar2 == '/');
  std::string res = part1;
  res.resize(res.length() - 1);
  res += part2;
  return res;
}

bool stringBegins(const std::string& str, const std::string& headToCheck, std::string& tail)
{
  assert(!headToCheck.empty());
  if (str.length() < headToCheck.length() || str.find(headToCheck) != 0)
    return 0;
  tail = str.substr(headToCheck.length());
  return 1;
}

#endif //FIXME_STRING_UTILS_H
