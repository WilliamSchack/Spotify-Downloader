#include "EnvironmentUtils.h"

#ifdef WIN32
std::string EnvironmentUtils::GetVariable(const std::string& key)
{
    return StringUtils::FromWString(GetVariableW(key));
}

std::wstring EnvironmentUtils::GetVariableW(const std::string& key)
#else
std::string EnvironmentUtils::GetVariable(const std::string& key)
#endif
{
#ifdef WIN32
    DWORD bufferSize = 65535;
    std::wstring buffer;
    buffer.resize(bufferSize);

    bufferSize = GetEnvironmentVariableW(StringUtils::ToWString(key).c_str(), &buffer[0], bufferSize);
    if (!bufferSize) {
        std::cout << "Could not retrieve environment variable: " << key << std::endl;
        return L"";
    }

    buffer.resize(bufferSize);
    return buffer;
#else
    char* value = std::getenv(key.c_str());
    if (value == NULL) {
        std::cout << "Could not retrieve environment variable: " << key << std::endl;
        return "";
    }

    return std::string(value);
#endif
}

std::filesystem::path EnvironmentUtils::GetVariablePath(const std::string& key)
{
#if WIN32
    std::wstring value = GetVariableW(key);
    if (value.empty())
        return "";

    return std::filesystem::path(value);
#else
    std::string value = GetVariable(key);
    if (value.empty())
        return "";

    return std::filesystem::path(GetVariable(key));
#endif
}