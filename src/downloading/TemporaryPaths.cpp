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

std::filesystem::path TemporaryPaths::GetTrackDownloadPathNoExtension(const TrackData& track)
{
    std::filesystem::path tempDownloadsFolder = GetDownloadsDir();

    std::string fileName = track.GetUniqueId();
    fileName = FileUtils::ValidateFileName(fileName);

    return tempDownloadsFolder / FileUtils::PathFromUtf8(fileName);
}

std::filesystem::path TemporaryPaths::GetTrackImagePathNoExtension(const TrackData& track)
{
    std::filesystem::path tempImagesFolder = GetImagesDir();

    std::string imageFileName = track.Album.GetUniqueId();
    imageFileName = FileUtils::ValidateFileName(imageFileName);

    return tempImagesFolder / FileUtils::PathFromUtf8(imageFileName);
}

std::filesystem::path TemporaryPaths::FindExistingTrackImagePath(const TrackData& track)
{
    std::filesystem::path imageFilePath = TemporaryPaths::GetTrackImagePathNoExtension(track);
    return FileUtils::FindPathWithAnyExtension(imageFilePath.parent_path(), imageFilePath.filename());
}