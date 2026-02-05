#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>
#include <cctype>
#include <algorithm>

class StringUtils
{
    public:
        static bool Contains(const std::string& string, const std::string& value);
        static void ToLower(std::string& string);
};

#endif