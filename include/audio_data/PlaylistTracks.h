#ifndef PLAYLISTTRACKS_H
#define PLAYLISTTRACKS_H

#include "PlaylistData.h"
#include "TrackData.h"

#include <vector>

struct PlaylistTracks
{
    PlaylistData Data;
    std::vector<TrackData> Tracks;

    PlaylistTracks() : Data(EPlatform::Unknown) {}
};

#endif