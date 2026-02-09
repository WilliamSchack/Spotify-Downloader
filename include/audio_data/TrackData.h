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
    std::string Description = "";
    std::string ReleaseDate = "";
    bool Explicit = false;
    unsigned int DurationMilliseconds = 0;
    unsigned int DurationSeconds = 0;
    unsigned int DiscNumber = 0;
    unsigned int TrackNumber = 0;
    unsigned int PlaylistTrackNumber = 0;
    AlbumData Album;
    std::vector<ArtistData> Artists;

    void SetDuration(unsigned int ms)
    {
        DurationMilliseconds = ms;
        DurationSeconds = ms / 1000;
    }
};

#endif