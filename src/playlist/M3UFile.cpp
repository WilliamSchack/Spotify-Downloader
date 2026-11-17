#include "M3UFile.h"

std::string M3UFile::GetExtension()
{
    return "m3u";
}

std::string M3UFile::WriteHeader()
{
    return "#EXTM3U\n";
}

std::string M3UFile::WriteFile(const PlaylistFileTrack& track)
{
    // Details
    std::string output = "#EXTINF:";
    output += std::to_string(track.Data.DurationSeconds) + ",";
    output += track.Data.GetMainArtist().Name + " - ";
    output += track.Data.Name + "\n";

    // Path
    output += track.EncodedPath + "\n";

    return output;
}

std::string M3UFile::WriteFooter()
{
    // No footer
    return "";
}
