#include "StringUtils.h"

bool StringUtils::StartsWith(const std::string& string, const std::string& value)
{
    return string.rfind(value, 0) == 0;
}

bool StringUtils::Contains(const std::string& string, const std::string& value)
{
    return string.find(value) != std::string::npos;
}

void StringUtils::ToLower(std::string& string)
{
    std::transform(string.begin(), string.end(), string.begin(),
        [](unsigned char c){ return std::tolower(c); });
}

void StringUtils::RemoveChar(std::string& string, const char& c)
{
    string.erase(std::remove(string.begin(), string.end(), c), string.end());
}

std::string StringUtils::RemoveLast(const std::string& string)
{
    return string.substr(0, string.size() - 1);
}

std::vector<std::string> StringUtils::Split(const std::string& string, const std::string& delimiter)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = string.find(delimiter);
    
    while (end != std::string::npos) {
        tokens.push_back(string.substr(start, end - start));
        start = end + 1;
        end = string.find(delimiter, start);
    }
    
    tokens.push_back(string.substr(start));
    return tokens;
}