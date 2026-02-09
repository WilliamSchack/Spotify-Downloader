#ifndef ALBUMDATA_H
#define ALBUMDATA_H

#include "EAlbumType.h"
#include "ArtistData.h"

#include <string>

#include <iostream>

struct AlbumData
{
    std::string Id = "";
    std::string Name = "";
    std::string Description = "";
    unsigned int TotalTracks = 0;
    std::string ImageUrl = "";
    std::string ReleaseDate = "";
    std::string ReleaseYear = "";
    EAlbumType Type = EAlbumType::Album;
    ArtistData MainArtist;

    void Print()
    {
        std::cout << Id << std::endl;
        std::cout << Name << std::endl;
        std::cout << Description << std::endl;
        std::cout << TotalTracks << std::endl;
        std::cout << ImageUrl << std::endl;
        std::cout << ReleaseDate << std::endl;
        std::cout << ReleaseYear << std::endl;
        std::cout << (int)Type << std::endl;
        std::cout << MainArtist.Id << std::endl;
        std::cout << MainArtist.Name << std::endl;
    }
};

#endif