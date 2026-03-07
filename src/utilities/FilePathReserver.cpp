#include "FilePathReserver.h"

FilePathReserver::~FilePathReserver()
{
    if (_path.empty()) return;

    // Unreserve path
    std::lock_guard<std::mutex> lock(_reservingPathMutex);
    VectorUtils::Remove(_reservedPaths, _path);
}

std::filesystem::path FilePathReserver::FindAvailableTrackPath(const TrackData& track, const std::filesystem::path& originalPath)
{
    // Lock other threads from calling this, if multiple threads call the same file path it will not work correctly
    std::lock_guard<std::mutex> lock(_reservingPathMutex);

    // If the file hasnt been used, use that
    bool fileExists = std::filesystem::exists(originalPath);
    if (!fileExists && !VectorUtils::Contains(_reservedPaths, originalPath)) {
        SetPath(originalPath);
        return originalPath;
    }

    bool sameTrack = false;
    if (fileExists) {
        // If the file doesnt exist assume another thread is working on a different track
        
        // Check the metadata and see it its the same
        MetadataManager existingMetadata(originalPath);
        sameTrack = existingMetadata.GetTitle() == track.Name &&
            existingMetadata.GetAlbumName() == track.Album.Name &&
            existingMetadata.GetTrackNumber() == track.TrackNumber &&
            (track.Artists.size() == 0 ? true : existingMetadata.GetArtist() == MetadataManager::CombineArtistNames(track.Artists)) &&
            (track.Album.Artists.size() == 0 ? true : existingMetadata.GetAlbumArtist() == MetadataManager::CombineArtistNames(track.Album.Artists));

        existingMetadata.Close();
    }

    if (sameTrack) {
        // Return the original path, it will be handled outside this function
        SetPath(originalPath);
        return originalPath;
    }

    // Append the id to the file name
    std::string fileName = originalPath.stem().string();
    fileName += "_" + track.Id + originalPath.extension().string();

    std::filesystem::path newPath = originalPath;
    newPath.replace_filename(fileName);

    SetPath(newPath);
    return newPath;
}

void FilePathReserver::SetPath(const std::filesystem::path& path)
{
    _path = path;

    // No need to lock, already locked in FindAvailableTrackPath
    _reservedPaths.push_back(_path);
}