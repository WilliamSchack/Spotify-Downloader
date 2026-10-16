#include "Paths.h"

std::filesystem::path Paths::GetConfigDir()
{
    std::filesystem::path path = "";

#ifdef WIN32
    EnvironmentVariable<std::filesystem::path> appdataPath = EnvironmentUtils::GetVariablePath("APPDATA");
    path = appdataPath.Value / APP_NAME;
#elif __APPLE__
    // TODO: Test me
    EnvironmentVariable<std::filesystem::path> homePath = EnvironmentUtils::GetVariablePath("HOME");
    std::filesystem::path configPath = homePath.Value / ".config";
    path = configPath / APP_NAME;
#else
    // TODO: Test me
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