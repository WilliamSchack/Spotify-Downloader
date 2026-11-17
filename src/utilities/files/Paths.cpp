#include "Paths.h"

std::filesystem::path Paths::GetExecutablePath()
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

std::filesystem::path Paths::GetRelativePath(const std::filesystem::path& relativePath)
{
    return GetExecutablePath().parent_path() / relativePath;
}

std::filesystem::path Paths::GetConfigDir()
{
    std::filesystem::path path = "";

#ifdef _WIN32
    EnvironmentVariable<std::filesystem::path> appdataPath = EnvironmentUtils::GetVariablePath("APPDATA");
    path = appdataPath.Value / APP_NAME;
#elif __APPLE__
    EnvironmentVariable<std::filesystem::path> homePath = EnvironmentUtils::GetVariablePath("HOME");
    std::filesystem::path configPath = homePath.Value / ".config";
    path = configPath / APP_NAME;
#else
    EnvironmentVariable<std::filesystem::path> xgdConfigPath = EnvironmentUtils::GetVariablePath("XDG_CONFIG_HOME");
    std::filesystem::path configPath = xgdConfigPath.Value;
    if (!xgdConfigPath.Exists) {
        EnvironmentVariable<std::filesystem::path> homePath = EnvironmentUtils::GetVariablePath("HOME");
        configPath = homePath.Value / ".config";
    }

    path = configPath / APP_NAME;
#endif

    if (!std::filesystem::exists(path))
        std::filesystem::create_directories(path);

    return path;
}

std::filesystem::path Paths::GetCacheDir()
{
    std::filesystem::path cacheFolder = GetConfigDir() / CACHE_FOLDER_NAME;
    if (!std::filesystem::exists(cacheFolder))
        std::filesystem::create_directory(cacheFolder);

    return cacheFolder;
}

std::filesystem::path Paths::GetTempDir()
{
    std::filesystem::path tempFolder = std::filesystem::temp_directory_path() / APP_NAME;
    if (!std::filesystem::exists(tempFolder))
        std::filesystem::create_directories(tempFolder);

    return tempFolder;
}

std::filesystem::path Paths::GetDownloadsDir()
{
    std::filesystem::path downloadsFolder = GetTempDir() / DOWNLOADS_FOLDER_NAME;
    if (!std::filesystem::exists(downloadsFolder))
        std::filesystem::create_directory(downloadsFolder);

    return downloadsFolder;
}

std::filesystem::path Paths::GetImagesDir()
{
    std::filesystem::path imagesFolder = GetTempDir() / IMAGES_FOLDER_NAME;
    if (!std::filesystem::exists(imagesFolder))
        std::filesystem::create_directory(imagesFolder);

    return imagesFolder;
}

std::filesystem::path Paths::GetTrackDownloadPathNoExtension(const TrackData& track)
{
    std::filesystem::path tempDownloadsFolder = GetDownloadsDir();

    std::string fileName = track.GetUniqueId();
    fileName = FileUtils::ValidateFileName(fileName);

    return tempDownloadsFolder / FileUtils::PathFromUtf8(fileName);
}

std::filesystem::path Paths::GetTrackImagePathNoExtension(const TrackData& track)
{
    std::filesystem::path tempImagesFolder = GetImagesDir();

    std::string imageFileName = track.Album.GetUniqueId();
    imageFileName = FileUtils::ValidateFileName(imageFileName);

    return tempImagesFolder / FileUtils::PathFromUtf8(imageFileName);
}

std::filesystem::path Paths::FindExistingTrackImagePath(const TrackData& track)
{
    std::filesystem::path imageFilePath = Paths::GetTrackImagePathNoExtension(track);
    return FileUtils::FindPathWithAnyExtension(imageFilePath.parent_path(), imageFilePath.filename());
}