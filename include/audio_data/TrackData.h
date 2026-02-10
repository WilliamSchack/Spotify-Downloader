#ifndef TRACKDATA_H
#define TRACKDATA_H

#include "EPlatform.h"
#include "ArtistData.h"
#include "AlbumData.h"

#include <string>
#include <vector>

#include <iostream>

struct TrackData
{
    EPlatform Platform = EPlatform::Unknown;
    std::string Id = "";
    std::string Url = "";
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

    TrackData(EPlatform platform) : Platform(platform), Album(platform) {}

    void SetDuration(unsigned int ms)
    {
        DurationMilliseconds = ms;
        DurationSeconds = ms / 1000;
    }

    void Print(bool printAlbum = true, bool printArtists = true)
    {
        std::cout << (int)Platform << std::endl;
        std::cout << Id << std::endl;
        std::cout << Url << std::endl;
        std::cout << Name << std::endl;
        std::cout << ReleaseDate << std::endl;
        std::cout << Explicit << std::endl;
        std::cout << DurationMilliseconds << std::endl;
        std::cout << DurationSeconds << std::endl;
        std::cout << DiscNumber << std::endl;
        std::cout << TrackNumber << std::endl;
        std::cout << PlaylistTrackNumber << std::endl;
        
        if (printAlbum) {
            std::cout << "ALBUM:" << std::endl;
            Album.Print();
        }

        if (printArtists) {
            std::cout << "ARTISTS:" << std::endl;
            for (ArtistData artist : Artists) {
                artist.Print();
            }
        }
    }
};

#endif