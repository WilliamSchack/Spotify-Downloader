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
        static std::string ValidateDirectoryName(const std::string& fileName);

        static std::string PathToUtf8(const std::filesystem::path& path);
        static std::filesystem::path PathFromUtf8(const std::string& path);

        static FILE* OpenFile(const std::filesystem::path& path, const std::string& mode);

        static std::filesystem::path GetExecutablePath();
    private:
        inline static const std::string INVALID_FILE_CHARS = R"(<>:"/\|?*)";
};

#endif