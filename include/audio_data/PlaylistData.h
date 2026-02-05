#ifndef PLAYLISTDATA_H
#define PLAYLISTDATA_H

#include "TrackData.h"

#include <string>
#include <vector>

struct PlaylistData
{
    std::string Id = "";
    std::string Name = "";
    int TotalTracks = 0;
    std::string ImageUrl = "";
    std::string Description = "";
    std::string OwnerId = "";
    std::string OwnerName = "";
    std::vector<TrackData> Tracks;
};

#endif