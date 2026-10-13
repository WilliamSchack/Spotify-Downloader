#include "IPlaylistFile.h"

void IPlaylistFile::CreatePlaylistFileFromTracks(const std::vector<std::filesystem::path>& trackFilePaths, const std::filesystem::path& outputPathWithoutExtension)
{
    // Setup the path
    std::string outputPathString = FileUtils::PathToUtf8(outputPathWithoutExtension) + "." + GetExtension();
    std::filesystem::path outputPath = FileUtils::PathFromUtf8(outputPathString);
    
    std::filesystem::path outputPathDir = outputPath.parent_path();
    if (!std::filesystem::is_directory(outputPathDir))
        std::filesystem::create_directories(outputPathDir);
    
    // Get & setup tracks
    std::vector<PlaylistFileTrack> tracks;
    for (std::filesystem::path path : trackFilePaths) {
        if (!std::filesystem::exists(path))
            continue;

        PlaylistFileTrack track;
        track.Path = path;
        track.EncodedPath = FileUtils::EncodePath(path);
        
        MetadataManager metadata(path);
        track.Data = metadata.GetAll();
    }

    if (tracks.size() == 0)
        return;

    // Sort by track number
    std::sort(tracks.begin(), tracks.end(), [](const PlaylistFileTrack t1, const PlaylistFileTrack t2) -> bool {
		return (t1.Data.TrackNumber < t2.Data.TrackNumber);
	});

    // Create & write the file
    std::ofstream file(outputPath);

    file << WriteHeader();

    for (PlaylistFileTrack track : tracks) {
        file << WriteFile(track);
    }

    file << WriteFooter();

    file.close();
}

void IPlaylistFile::CreatePlaylistFileFromDirectory(const std::filesystem::path& tracksFolder, const std::filesystem::path& outputPathWithoutExtension)
{
    std::vector<std::filesystem::path> filePaths;

    std::vector<std::string> filters = GetFilters();
    std::filesystem::recursive_directory_iterator iterator(tracksFolder);
    for (const std::filesystem::directory_entry& dirEntry : iterator) {
        if (!dirEntry.is_regular_file())
            continue;
        
        std::filesystem::path path = dirEntry.path();
        if (!path.has_extension())
        continue;
        
        std::string extensionLower = StringUtils::ToLower(FileUtils::PathToUtf8(path.extension()));
        extensionLower = StringUtils::RemoveFirst(extensionLower); // remove .
        if (!VectorUtils::Contains(filters, extensionLower))
            continue;
        
        filePaths.push_back(path);
    }

    CreatePlaylistFileFromTracks(filePaths, outputPathWithoutExtension);
}

std::vector<std::string> IPlaylistFile::GetFilters()
{
    if (!_filtersCache.empty())
        return _filtersCache;

    std::vector<std::string> filters;
    for (std::string extension : EEXTENSION_NAMES) {
        filters.push_back(StringUtils::ToLower(extension));
    }

    _filtersCache = filters;
    return filters;
}
