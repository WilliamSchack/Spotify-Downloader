#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "StringUtils.h"

#include <iostream>
#include <string>
#include <filesystem>

#ifdef WIN32
    #include <windows.h>
#elif __APPLE__
    #include <mach-o/dyld.h>
#else
    #include <unistd.h>
    #include <limits.h>
#endif

class FileUtils
{
    public:
        static std::string ValidateFileName(const std::string& fileName);
        static std::wstring ValidateFileName(const std::wstring& fileName);
        static std::string ValidateDirectoryName(const std::string& fileName);
        static std::filesystem::path GetExecutablePath();
    private:
        inline static const std::string INVALID_FILE_CHARS = R"(<>:"/\|?*)";
};

#endif