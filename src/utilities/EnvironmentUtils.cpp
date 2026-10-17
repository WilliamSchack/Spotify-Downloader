#include "EnvironmentUtils.h"

#ifdef WIN32
EnvironmentVariable<std::string> EnvironmentUtils::GetVariable(const std::string& key)
{
    EnvironmentVariable<std::string> output;

    EnvironmentVariable<std::wstring> variable = GetVariableW(key);
    if (!variable.Exists) {
        output.Value = "";
        output.Exists = false;
        return output;
    }

    output.Value = StringUtils::FromWString(variable.Value);
    output.Exists = true;
    return output;
}

EnvironmentVariable<std::wstring> EnvironmentUtils::GetVariableW(const std::string& key)
#else
EnvironmentVariable<std::string> EnvironmentUtils::GetVariable(const std::string& key)
#endif
{
#ifdef WIN32
    EnvironmentVariable<std::wstring> output;

    DWORD bufferSize = 65535;
    std::wstring buffer;
    buffer.resize(bufferSize);

    bufferSize = GetEnvironmentVariableW(StringUtils::ToWString(key).c_str(), &buffer[0], bufferSize);
    if (!bufferSize) {
        std::cout << "Could not retrieve environment variable: " << key << std::endl;

        output.Value = L"";
        output.Exists = false;
        return output;
    }

    buffer.resize(bufferSize);

    output.Value = buffer;
    output.Exists = true;
    return output;
#else
    EnvironmentVariable<std::string> output;

    char* value = std::getenv(key.c_str());
    if (value == NULL) {
        std::cout << "Could not retrieve environment variable: " << key << std::endl;

        output.Value = "";
        output.Exists = false;
        return output;
    }

    output.Value = std::string(value);
    output.Exists = true;
    return output;
#endif
}

EnvironmentVariable<std::filesystem::path> EnvironmentUtils::GetVariablePath(const std::string& key)
{
    EnvironmentVariable<std::filesystem::path> output;

#if WIN32
    EnvironmentVariable<std::wstring> variable = GetVariableW(key);
    if (!variable.Exists) {
        output.Value = "";
        output.Exists = false;
        return output;
    }

    output.Value = std::filesystem::path(variable.Value);
    output.Exists = true;
    return output;
#else
    EnvironmentVariable<std::string> variable = GetVariable(key);
    if (!variable.Exists) {
        output.Value = "";
        output.Exists = false;
        return output;
    }

    output.Value = std::filesystem::path(variable.Value);
    output.Exists = true;
    return output;
#endif
}