#ifndef PLAYLISTDATA_H
#define PLAYLISTDATA_H

#include "EPlatform.h"
#include "TrackData.h"
#include "ArtistData.h"

#include <string>
#include <vector>

#include <iostream>

struct PlaylistData
{
    EPlatform Platform = EPlatform::Unknown;
    std::string Id = "";
    std::string Url = "";
    std::string Name = "";
    std::string Description = "";
    std::string ImageUrl = "";
    unsigned int TotalTracks = 0;
    ArtistData Owner;

    PlaylistData(EPlatform platform) : Platform(platform), Owner(platform) {}

    void Print()
    {
        std::cout << (int)Platform << std::endl;
        std::cout << Id << std::endl;
        std::cout << Url << std::endl;
        std::cout << Name << std::endl;
        std::cout << Description << std::endl;
        std::cout << ImageUrl << std::endl;
        std::cout << TotalTracks << std::endl;
        std::cout << "OWNER:" << std::endl;
        Owner.Print();
    }
};

#endif