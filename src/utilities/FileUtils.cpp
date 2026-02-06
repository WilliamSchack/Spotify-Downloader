#include "FileUtils.h"

std::string FileUtils::ValidateFileName(const std::string& fileName)
{
    std::string copiedString = fileName;
    std::string invalidChars = R"(<>:"/\|?*)";

    for (const char c : invalidChars) {
        StringUtils::RemoveChar(copiedString, c);
    }

    return copiedString;
};

std::string FileUtils::ValidateDirectoryName(const std::string& directoryName)
{
    return ValidateFileName(directoryName);
};
