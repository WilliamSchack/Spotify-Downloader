#ifndef PLAYLISTDATA_H
#define PLAYLISTDATA_H

#include "TrackData.h"
#include "ArtistData.h"

#include <string>
#include <vector>

#include <iostream>

struct PlaylistData
{
    std::string Id = "";
    std::string Name = "";
    std::string Description = "";
    std::string ImageUrl = "";
    unsigned int TotalTracks = 0;
    ArtistData Owner;

    void Print()
    {
        std::cout << Id << std::endl;
        std::cout << Name << std::endl;
        std::cout << Description << std::endl;
        std::cout << ImageUrl << std::endl;
        std::cout << TotalTracks << std::endl;
        std::cout << "OWNER:" << std::endl;
        Owner.Print();
    }
};

#endif