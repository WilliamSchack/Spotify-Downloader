#ifndef ALBUM_H
#define ALBUM_H

#include "EAlbumType.h"
#include "Artist.h"

#include <string>
#include <vector>

struct Album
{
    std::string Id = "";
    int TotalTracks = 0;
    std::string ReleaseDate = "";
    std::string ReleaseDatePrecision = "";
    EAlbumType Type = EAlbumType::Album;
    std::vector<Artist> Artists;
};

#endif