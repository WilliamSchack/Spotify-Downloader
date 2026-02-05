#ifndef ALBUM_H
#define ALBUM_H

#include "EAlbumType.h"
#include "ArtistData.h"

#include <string>
#include <vector>

struct AlbumData
{
    std::string Id = "";
    int TotalTracks = 0;
    std::string ReleaseDate = "";
    std::string ReleaseDatePrecision = "";
    EAlbumType Type = EAlbumType::Album;
    std::vector<ArtistData> Artists;
};

#endif