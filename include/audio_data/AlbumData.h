#ifndef ALBUMDATA_H
#define ALBUMDATA_H

#include "EAlbumType.h"
#include "ArtistData.h"

#include <string>
#include <vector>

struct AlbumData
{
    std::string Id = "";
    std::string Name = "";
    std::string Description = "";
    unsigned int TotalTracks = 0;
    std::string ImageUrl = "";
    std::string ReleaseDate = "";
    std::string ReleaseDatePrecision = "";
    EAlbumType Type = EAlbumType::Album;
    std::vector<ArtistData> Artists;
};

#endif