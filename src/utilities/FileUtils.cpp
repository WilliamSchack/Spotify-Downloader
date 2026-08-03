#include "FileUtils.h"

std::string FileUtils::ValidateFileName(const std::string& fileName)
{
    std::string copiedString = fileName;
    for (const char c : INVALID_FILE_CHARS) {
        StringUtils::RemoveChar(copiedString, c);
    }

    return copiedString;
};

std::string FileUtils::ValidateDirectoryName(const std::string& directoryName)
{
    return ValidateFileName(directoryName);
};

std::string FileUtils::PathToUtf8(const std::filesystem::path& path)
{
#ifdef WIN32
    return StringUtils::FromWString(path.wstring());
#else
    return path.string();
#endif
}

std::filesystem::path FileUtils::PathFromUtf8(const std::string& path)
{
#ifdef WIN32
    return std::filesystem::path(StringUtils::ToWString(path));
#else
    return std::filesystem::path(path);
#endif
}

FILE* FileUtils::OpenFile(const std::filesystem::path& path, const std::string& mode)
{
#ifdef WIN32
    return _wfopen(path.wstring().c_str(), StringUtils::ToWString(mode).c_str());
#else
    return fopen(path.c_str(), mode.c_str());
#endif
}


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