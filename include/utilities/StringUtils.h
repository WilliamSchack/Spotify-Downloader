#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>
#include <cctype>
#include <vector>
#include <algorithm>

class StringUtils
{
    public:
        static bool StartsWith(const std::string& string, const std::string& value);
        static bool Contains(const std::string& string, const std::string& value);
        static void ToLower(std::string& string);
        static void RemoveChar(std::string& string, const char& c);
        static std::string RemoveLast(const std::string& string);
        static std::vector<std::string> Split(const std::string& string, const std::string& delimiter);
};

#endif