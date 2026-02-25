#ifndef ILYRICSPLATFORM_H
#define ILYRICSPLATFORM_H

#include "Lyrics.h"
#include "TrackData.h"

class ILyricsPlatform
{
    public:
        virtual ~ILyricsPlatform() = default;

        virtual Lyrics GetLyrics(const TrackData& track) = 0;
};

#endif