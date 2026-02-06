#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "StringUtils.h"

#include <string>

class FileUtils
{
    public:
        static std::string ValidateFileName(const std::string& fileName);
        static std::string ValidateDirectoryName(const std::string& fileName);
};

#endif