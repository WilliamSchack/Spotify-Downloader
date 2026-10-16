#ifndef ENVIRONMENTUTILS_H
#define ENVIRONMENTUTILS_H

#include "FileUtils.h"
#include "StringUtils.h"

class EnvironmentUtils
{
    public:
#ifdef WIN32
        static std::wstring GetVariableW(const std::string& key);
#endif
        static std::string GetVariable(const std::string& key);

        static std::filesystem::path GetVariablePath(const std::string& key);
};

#endif