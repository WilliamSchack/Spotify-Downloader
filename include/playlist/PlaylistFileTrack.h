#ifndef PLAYLISTFILETRACK_H
#define PLAYLISTFILETRACK_H

#include "TrackData.h"

#include <filesystem>

struct PlaylistFileTrack
{
    std::filesystem::path Path = "";
    std::string EncodedPath = "";

    TrackData Data;

    PlaylistFileTrack() : Data(EPlatform::Unknown) {}
};

#endif