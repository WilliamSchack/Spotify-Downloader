#ifndef PLAYLISTDATA_H
#define PLAYLISTDATA_H

#include "TrackData.h"
#include "ArtistData.h"

#include <string>
#include <vector>

struct PlaylistData
{
    std::string Id = "";
    std::string Name = "";
    unsigned int TotalTracks = 0;
    std::string ImageUrl = "";
    std::string Description = "";
    ArtistData Owner;
};

#endif