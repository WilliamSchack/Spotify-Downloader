#ifndef LYRICSFINDER_H
#define LYRICSFINDER_H

#include "Lyrics.h"
#include "ELyricsType.h"
#include "TrackData.h"

class LyricsFinder
{
    public:
        static Lyrics GetLyrics(const TrackData& track, const ELyricsType& lyricsType);

        // Gets the lyrics from the source platform if it exists
        static Lyrics GetSourceLyrics(const TrackData& source);

        // Gets all the lyrics in order of the config priority
        static Lyrics GetPriorityLyrics(const TrackData& track);
};

#endif