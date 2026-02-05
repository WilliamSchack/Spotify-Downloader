#ifndef TRACKDATA_H
#define TRACKDATA_H

#include "ArtistData.h"
#include "AlbumData.h"

#include <string>
#include <vector>

struct TrackData
{
    std::string Id = "";
    std::string Isrc = "";
    std::string Name = "";
    bool Explicit = false;
    int DurationMs = 0;
    int DurationS = 0;
    int DiscNumber = 0;
    int TrackNumber = 0;
    int PlaylistTrackNumber = 0;
    AlbumData Album;
    std::vector<ArtistData> Artists;

    void SetDuration(int ms)
    {
        DurationMs = ms;
        DurationS = ms / 1000;
    }
};

#endif