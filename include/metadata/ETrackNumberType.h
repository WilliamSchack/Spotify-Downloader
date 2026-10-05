#ifndef ETRACKNUMBERTYPE_H
#define ETRACKNUMBERTYPE_H

#include <vector>
#include <string>

enum class ETrackNumberType
{
    Playlist,
    Album,
    Disc
};

static inline const std::vector<std::string> ETRACKNUMBERTYPE_NAMES = {
    "Playlist",
    "Album",
    "Disc"
};

#endif