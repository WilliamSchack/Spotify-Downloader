#include "YoutubeLyrics.h"

YoutubeLyrics::YoutubeLyrics() : _yt() {}

Lyrics YoutubeLyrics::GetLyrics(const TrackData& track)
{
    // Could search for the track but this will only be used when the track is from youtube so no need
    if (track.Platform != EPlatform::YouTube)
        return Lyrics();

    return _yt.GetLyrics(track.Id);
}