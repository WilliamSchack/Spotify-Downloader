#ifndef ENVIRONMENTUTILS_H
#define ENVIRONMENTUTILS_H

#include "EnvironmentVariable.h"
#include "StringUtils.h"

#include <filesystem>
#include <iostream>

#ifdef WIN32
#include <windows.h>
#endif

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