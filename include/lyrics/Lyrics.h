#ifndef LYRICS_H
#define LYRICS_H

#include "ELyricsSource.h"
#include "ELyricsType.h"
#include "SynchronisedLyric.h"
#include "StringUtils.h"

#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

struct Lyrics
{
    ELyricsType Type = ELyricsType::None;

    ELyricsSource Source = ELyricsSource::Unknown;
    std::string SourceMessage = "";

    std::string UnsyncedLyrics = "";
    std::vector<SynchronisedLyric> SyncedLyrics;

    Lyrics(const ELyricsSource& source);

    std::string GetString() const;
};

#endif