#include "LyricsFile.h"

bool LyricsFile::Create(const std::string& outputPathNoExtension, const TrackData& trackData, const Lyrics& lyrics)
{
    if (lyrics.Type == ELyricsType::None)
        return false;

    std::string outputPathString = FileUtils::ValidateDirectoryName(outputPathNoExtension) + ".lrc";
    std::filesystem::path outputPath = FileUtils::PathFromUtf8(outputPathString);

    // Create the folder if it doesnt exist
    std::filesystem::path outputPathDir = outputPath.parent_path();
    if (!std::filesystem::is_directory(outputPathDir))
        std::filesystem::create_directories(outputPathDir);

    // Create and open the file
    std::ofstream file(outputPath);

    // Write the header
    file << "[ar:" << MetadataManager::CombineArtistNames(trackData.Artists, ", ") << "]" << std::endl;
    file << "[al:" << trackData.Album.Name << "]" << std::endl;
    file << "[ti:" << trackData.Name << "]" << std::endl;
    file << "[length: " << trackData.GetDurationMinutes() << "]" << std::endl;
    file << std::endl;

    // Write lyrics
    file << lyrics.GetString();

    file.close();

    return true;
}