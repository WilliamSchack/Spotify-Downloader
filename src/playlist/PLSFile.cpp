#include "PLSFile.h"

std::string PLSFile::GetExtension()
{
    return "pls";
}

std::string PLSFile::WriteHeader()
{
    _currentFile = 1;
    return "[playlist]\n\n";
}

std::string PLSFile::WriteFile(const PlaylistFileTrack& track)
{
    std::string currentFileString = std::to_string(_currentFile);

    std::string output;
    output += "File" + currentFileString + "=" + track.EncodedPath + "\n";
    output += "Title" + currentFileString + "=" + track.Data.Name + "\n";
    output += "Length" + currentFileString + "=" + std::to_string(track.Data.DurationSeconds) + "\n\n";

    _currentFile++;
    return output;
}

std::string PLSFile::WriteFooter()
{
    std::string output;
    output += "NumberOfEntries=" + std::to_string(_currentFile - 1);
    output += "\nVersion=2";

    return output;
}
