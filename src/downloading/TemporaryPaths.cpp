#include "TemporaryPaths.h"

std::filesystem::path TemporaryPaths::GetTempDir()
{
    std::filesystem::path tempFolder = std::filesystem::temp_directory_path() / APP_NAME;
    if (!std::filesystem::exists(tempFolder))
        std::filesystem::create_directory(tempFolder);

    return tempFolder;
}

std::filesystem::path TemporaryPaths::GetDownloadsDir()
{
    std::filesystem::path downloadsFolder = GetTempDir() / DOWNLOADS_FOLDER_NAME;
    if (!std::filesystem::exists(downloadsFolder))
        std::filesystem::create_directory(downloadsFolder);

    return downloadsFolder;
}

std::filesystem::path TemporaryPaths::GetImagesDir()
{
    std::filesystem::path imagesFolder = GetTempDir() / IMAGES_FOLDER_NAME;
    if (!std::filesystem::exists(imagesFolder))
        std::filesystem::create_directory(imagesFolder);

    return imagesFolder;
}

std::filesystem::path TemporaryPaths::GetTrackDownloadPath(const TrackData& track, const ICodec& codec)
{
    std::filesystem::path tempDownloadsFolder = GetDownloadsDir();

    std::string fileName = track.GetUniqueId() + "." + codec.GetString();
    fileName = FileUtils::ValidateFileName(fileName);

    return tempDownloadsFolder / FileUtils::PathToUtf8(fileName);
}

std::filesystem::path TemporaryPaths::GetTrackImagePath(const TrackData& track)
{
    std::filesystem::path tempImagesFolder = GetImagesDir();

    std::string imageFileName = track.Album.GetUniqueId();
    imageFileName = FileUtils::ValidateFileName(imageFileName);

    return tempImagesFolder / FileUtils::PathFromUtf8(imageFileName);
}