#include "FileUtils.h"

std::string FileUtils::ValidateFileName(const std::string& fileName)
{
    std::string copiedString = fileName;
    for (const char c : INVALID_FILE_CHARS) {
        copiedString = StringUtils::RemoveChar(copiedString, c);
    }

    return copiedString;
};

std::string FileUtils::ValidateDirectory(const std::string& directoryName)
{
    std::string copiedString = directoryName;
    for (const char c : INVALID_DIR_CHARS) {
        copiedString = StringUtils::RemoveChar(copiedString, c);
    }

    // Remove double seperators (//, \\)
    copiedString = StringUtils::ReplaceAll(copiedString, "//", "/");
    copiedString = StringUtils::ReplaceAll(copiedString, "\\\\", "\\");

#ifndef _WIN32
    // Replace home dir
    if (StringUtils::StartsWith(copiedString, "~/") || StringUtils::StartsWith(copiedString, "~\\")) {
        EnvironmentVariable<std::filesystem::path> homePath = EnvironmentUtils::GetVariablePath("HOME");
        if (homePath.Exists) {
            std::string homePathString = PathToUtf8(homePath.Value);
            copiedString = homePathString + "/" + copiedString.substr(2, copiedString.length() - 2);
        }
    }
#endif

    // Insert backspaces
    while (StringUtils::Contains(copiedString, "..")) {
        int startIndex = copiedString.find_first_of("..");
        int endIndex = startIndex + 2;

        int previousSlashIndexFirst = StringUtils::GetLastIndexOfAny(copiedString.substr(0, startIndex), { '/', '\\' });
        int previousSlashIndexSecond = StringUtils::GetLastIndexOfAny(copiedString.substr(0, previousSlashIndexFirst), { '/', '\\' });

        // If either of the previous slashes are not found, remove this backspace
        if (previousSlashIndexFirst == std::string::npos || previousSlashIndexSecond == std::string::npos) {
            copiedString = copiedString.substr(endIndex, copiedString.length() - endIndex);
            continue;
        }

        // Remove the previous folder and this backspace
        std::string newCopiedString = copiedString.substr(0, previousSlashIndexSecond);
        newCopiedString += copiedString.substr(endIndex, copiedString.length() - endIndex);
        copiedString = newCopiedString;
    }

    return copiedString;
};

std::string FileUtils::PathToUtf8(const std::filesystem::path& path)
{
#ifdef _WIN32
    return StringUtils::FromWString(path.wstring());
#else
    return path.string();
#endif
}

std::filesystem::path FileUtils::PathFromUtf8(const std::string& path)
{
#ifdef _WIN32
    return std::filesystem::path(StringUtils::ToWString(path));
#else
    return std::filesystem::path(path);
#endif
}

std::string FileUtils::EncodePath(const std::filesystem::path& path)
{
    std::string pathString = PathToUtf8(path);

    std::regex whitespaceRegex(R"([\s]+)");
    pathString = std::regex_replace(pathString, whitespaceRegex, "%20");
    
    return pathString;
}

FILE* FileUtils::OpenFile(const std::filesystem::path& path, const std::string& mode)
{
#ifdef _WIN32
    return _wfopen(path.wstring().c_str(), StringUtils::ToWString(mode).c_str());
#else
    return fopen(path.c_str(), mode.c_str());
#endif
}

bool FileUtils::MoveFile(const std::filesystem::path& fromPath, const std::filesystem::path& toPath)
{
    try {
        std::filesystem::rename(fromPath, toPath);
    } catch (...) {
        // If rename fails, assume it is between drives, in that case copy
        bool copied = std::filesystem::copy_file(fromPath, toPath);
        std::filesystem::remove(fromPath);

        // Move error
        if (!copied) {
            Logger::Warning("Failed to move the file from " + PathToUtf8(fromPath) + " to " + PathToUtf8(toPath));
            return false;
        }
    }

    return true;
}

std::filesystem::path FileUtils::GetExecutablePath()
{
#ifdef _WIN32
    char buffer[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if (len == 0) {
        std::ostringstream errorSs;
        errorSs << "Could not get executable path. Error: " << GetLastError();
        Logger::Warning(errorSs.str());
        return {};
    } else if (len == MAX_PATH) {
        Logger::Warning("Path truncated, file path is too long.");
        return {};
    }

    return std::filesystem::path(buffer);
#elif __APPLE__
    char buffer[PATH_MAX];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) != 0) {
        Logger::Warning("Could not get executable path");
        return {};
    }

    return std::filesystem::path(buffer);
#else
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len == -1) {
        Logger::Warning("Could not get executable path");
        return {};
    }

    buffer[len] = '\0';
    return std::filesystem::path(buffer);
#endif
}

std::filesystem::path FileUtils::GetRelativePath(const std::filesystem::path& relativePath)
{
    return GetExecutablePath().parent_path() / relativePath;
}

// TODO: Might be a good idea to cache this, just check it with many files in the target directory
std::filesystem::path FileUtils::FindPathWithAnyExtension(const std::filesystem::path& directory, const std::filesystem::path& fileName)
{
    if (!std::filesystem::exists(directory))
        return {};

    for (const std::filesystem::directory_entry& dirEntry : std::filesystem::directory_iterator(directory)) {
        if (!dirEntry.is_regular_file())
            continue;

        if (dirEntry.path().stem() == fileName)
            return dirEntry.path();
    }

    return {};
}