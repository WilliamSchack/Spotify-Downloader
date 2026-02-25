#ifndef LYRICS_H
#define LYRICS_H

#include "ELyricsSource.h"
#include "ELyricsType.h"
#include "SynchronisedLyric.h"

#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

struct Lyrics
{
    ELyricsType Type;

    ELyricsSource Source;
    std::string SourceMessage;

    std::string UnsyncedLyrics;
    std::vector<SynchronisedLyric> SyncedLyrics;

    std::string GetString() const;
};

#endif