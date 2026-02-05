#ifndef PLAYLISTDATA_H
#define PLAYLISTDATA_H

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
};

#endif