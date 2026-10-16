#ifndef ENVIRONMENTUTILS_H
#define ENVIRONMENTUTILS_H

#include "EnvironmentVariable.h"
#include "FileUtils.h"
#include "StringUtils.h"

class EnvironmentUtils
{
    public:
#ifdef WIN32
        static EnvironmentVariable<std::wstring> GetVariableW(const std::string& key);
#endif
        static EnvironmentVariable<std::string> GetVariable(const std::string& key);

        static EnvironmentVariable<std::filesystem::path> GetVariablePath(const std::string& key);
};

#endif