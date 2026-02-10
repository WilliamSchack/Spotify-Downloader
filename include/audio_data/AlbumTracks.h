#ifndef ALBUMTRACKS_H
#define ALBUMTRACKS_H

#include "AlbumData.h"
#include "TrackData.h"

#include <vector>

struct AlbumTracks
{
    AlbumData Data;
    std::vector<TrackData> Tracks;

    AlbumTracks() : Data(EPlatform::Unknown) {}
};

#endif