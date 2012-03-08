
#ifndef DEEPSOLVER_STRING_UTILS_H
#define DEEPSOLVER_STRING_UTILS_H

#define BLANK_CHAR(x) ((x)==10 || (x)==13 || (x)==9 || (x)==32)

bool hasNonSpaces(const std::string& s);
std::string concatUnixPath(const std::string& part1, const std::string& part2);
bool stringBegins(const std::string& str, const std::string& headToCheck, std::string& tail);
bool checkExtension(const std::string& fileName, const std::string& extension);
std::string trim(const std::string& str);

#endif //DEEPSOLVER_STRING_UTILS_H
