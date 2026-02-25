#ifndef LYRICS_H
#define LYRICS_H

#include "ELyricsSource.h"
#include "ELyricsType.h"
#include "SynchronisedLyric.h"

#include <string>
#include <vector>

struct Lyrics
{
    ELyricsSource Source;
    ELyricsType Type;

    std::string UnsynchedLyrics;
    std::vector<SynchronisedLyric> SynchedLyrics;

    std::string GetString() const;
};

#endif