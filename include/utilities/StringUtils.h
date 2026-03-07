#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>
#include <cctype>
#include <vector>
#include <algorithm>
#include <locale>
#include <codecvt>

class StringUtils
{
    public:
        static std::wstring ToWString(const std::string& string);
        static std::string FromWString(const std::wstring& string);
        static bool StartsWith(const std::string& string, const std::string& value);
        static bool Contains(const std::string& string, const std::string& value);
        static bool Contains(const std::wstring& string, const std::wstring& value);
        static void ToLower(std::string& string);
        static std::string ReplaceAll(std::string string, const std::string& from, const std::string& to);
        static void RemoveChar(std::string& string, const char& c);
        static void RemoveChar(std::wstring& string, const char& c);
        static std::string RemoveLast(const std::string& string);
        static std::vector<std::string> Split(const std::string& string, const std::string& delimiter);
        static double LevenshteinDistanceSimilarity(const std::string& s1, const std::string& s2);
        static unsigned int TimeToSeconds(const std::string& string);
        static unsigned int TimeToMilliseconds(const std::string& string);
};

#endif