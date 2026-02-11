#ifndef ALBUMDATA_H
#define ALBUMDATA_H

#include "EPlatform.h"
#include "EAlbumType.h"
#include "ArtistData.h"

#include <string>
#include <vector>

#include <iostream>

struct AlbumData
{
    EPlatform Platform = EPlatform::Unknown;
    std::string Id = "";
    std::string Url = "";
    std::string Name = "";
    std::string Description = "";
    unsigned int TotalTracks = 0;
    std::string ImageUrl = "";
    std::string ReleaseDate = "";
    std::string ReleaseYear = "";
    EAlbumType Type = EAlbumType::Album;
    std::vector<ArtistData> Artists;

    AlbumData(EPlatform platform) : Platform(platform) {}

    void SetMainArtist(ArtistData artist)
    {
        Artists = std::vector<ArtistData>();
        Artists.push_back(artist);
    }

    void Print(bool printArtists = true) const
    {
        std::cout << (int)Platform << std::endl;
        std::cout << Id << std::endl;
        std::cout << Url << std::endl;
        std::cout << Name << std::endl;
        std::cout << Description << std::endl;
        std::cout << TotalTracks << std::endl;
        std::cout << ImageUrl << std::endl;
        std::cout << ReleaseDate << std::endl;
        std::cout << ReleaseYear << std::endl;
        std::cout << (int)Type << std::endl;
        
        if (printArtists) {
            std::cout << "ARTISTS:" << std::endl;
            for (ArtistData artist : Artists) {
                artist.Print();
            }
        }
    }
};

#endif