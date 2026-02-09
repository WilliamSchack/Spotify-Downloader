#ifndef TRACKDATA_H
#define TRACKDATA_H

#include "ArtistData.h"
#include "AlbumData.h"

#include <string>
#include <vector>

#include <iostream>

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

    void Print()
    {
        std::cout << Id << std::endl;
        std::cout << Name << std::endl;
        std::cout << ReleaseDate << std::endl;
        std::cout << Explicit << std::endl;
        std::cout << DurationMilliseconds << std::endl;
        std::cout << DurationSeconds << std::endl;
        std::cout << DiscNumber << std::endl;
        std::cout << TrackNumber << std::endl;
        std::cout << PlaylistTrackNumber << std::endl;
        std::cout << "ALBUM:" << std::endl;
        Album.Print();
        std::cout << "ARTISTS:" << std::endl;
        for (ArtistData artist : Artists) {
            artist.Print();
        }
    }
};

#endif