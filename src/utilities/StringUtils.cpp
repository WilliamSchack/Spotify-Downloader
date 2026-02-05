#include "StringUtils.h"

bool StringUtils::Contains(const std::string& string, const std::string& value)
{
    return string.find(value) != std::string::npos;
}

void StringUtils::ToLower(std::string& string)
{
    std::transform(string.begin(), string.end(), string.begin(),
        [](unsigned char c){ return std::tolower(c); });
}