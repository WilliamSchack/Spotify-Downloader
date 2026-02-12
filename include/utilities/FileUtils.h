#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "StringUtils.h"

#include <iostream>
#include <string>
#include <filesystem>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <limits.h>
#endif

class FileUtils
{
    public:
        static std::string ValidateFileName(const std::string& fileName);
        static std::string ValidateDirectoryName(const std::string& fileName);
        static std::filesystem::path GetExecutablePath();
};

#endif