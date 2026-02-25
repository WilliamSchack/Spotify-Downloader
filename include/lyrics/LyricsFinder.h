#ifndef LYRICSFINDER_H
#define LYRICSFINDER_H

#include "LyricsPlatformFactory.h"
#include "Lyrics.h"
#include "TrackData.h"
#include "Config.h"

class LyricsFinder
{
    public:
        // Gets the lyrics from the source platform if it exists
        static Lyrics GetSourceLyrics(const TrackData& source);

        // Gets all the lyrics in order of the config priority
        static Lyrics GetBestLyrics(const TrackData& track);
};

#endif