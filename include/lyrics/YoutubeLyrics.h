#ifndef YOUTUBELYRICS_H
#define YOUTUBELYRICS_H

#include "ILyricsPlatform.h"
#include "YTMusicAPI.h"

class YoutubeLyrics : public ILyricsPlatform
{
    public:
        YoutubeLyrics();

        Lyrics GetLyrics(const TrackData& track) override;
    private:
        YTMusicAPI _yt;
};

#endif