#include "FileUtils.h"

std::string FileUtils::ValidateFileName(const std::string& fileName)
{
    std::string copiedString = fileName;
    for (const char c : INVALID_FILE_CHARS) {
        StringUtils::RemoveChar(copiedString, c);
    }

    return copiedString;
};

std::wstring FileUtils::ValidateFileName(const std::wstring& fileName)
{
    std::wstring copiedString(fileName.begin(), fileName.end());
    for (const char c : INVALID_FILE_CHARS) {
        StringUtils::RemoveChar(copiedString, c);
    }

    return copiedString;
};

std::string FileUtils::ValidateDirectoryName(const std::string& directoryName)
{
    return ValidateFileName(directoryName);
};

std::filesystem::path FileUtils::GetExecutablePath()
{
#ifdef WIN32
    char buffer[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if (len == 0) {
        std::cout << "Could not get executable path. Error: " << GetLastError() << std::endl;
        return {};
    } else if (len == MAX_PATH) {
        std::cout << "Path truncated, file path is too long." << std::endl;
        return {};
    }

    return std::filesystem::path(buffer);
#elif __APPLE__
    char buffer[PATH_MAX];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) != 0) {
        std::cout << "Could not get executable path" << std::endl;
        return {};
    }

    return std::filesystem::path(buffer);
#else
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len == -1) {
        std::cout << "Could not get executable path" << std::endl;
        return {};
    }

    buffer[len] = '\0';
    return std::filesystem::path(buffer);
#endif
}