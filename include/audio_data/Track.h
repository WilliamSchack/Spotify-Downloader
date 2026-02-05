#ifndef TRACK_H
#define TRACK_H

#include "Artist.h"
#include "Album.h"

#include <string>
#include <vector>

struct Track
{
    std::string Id = "";
    std::string Isrc = "";
    std::string Name = "";
    bool Explicit = false;
    int DurationMs = 0;
    int DiscNumber = 0;
    int TrackNumber = 0;
    int PlaylistTrackNumber = 0;
    Album Album;
    std::vector<Artist> Artists;
};

#endif