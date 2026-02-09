#ifndef ALBUMDATA_H
#define ALBUMDATA_H

#include "EAlbumType.h"
#include "ArtistData.h"

#include <string>

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
};

#endif