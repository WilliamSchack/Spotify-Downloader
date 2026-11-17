#ifndef EPLAYLISTFILETYPE_H
#define EPLAYLISTFILETYPE_H

#include <vector>
#include <string>

enum class EPlaylistFileType
{
    None,
    M3U,
    XSPF,
    PLS
};

static inline const std::vector<std::string> EPLAYLISTFILETYPE_NAMES = {
    "None",
    "M3U",
    "XSPF",
    "PLS"
};

#endif