#include "YoutubeLyrics.h"

YoutubeLyrics::YoutubeLyrics() : _yt() {}

#include <iostream>

Lyrics YoutubeLyrics::GetLyrics(const TrackData& track)
{
    if (track.Platform != EPlatform::YouTube) {
        // Get the track on youtube if the input one is from a different platform
        std::unique_ptr<IPlatformSearcher> searcher = PlatformFactory::CreateSearcher(EPlatform::YouTube);
        if (searcher == nullptr) return Lyrics(ELyricsSource::YouTube);

        PlatformSearcherResult searchResult = searcher->FindTrack(track);
        if (searchResult.Data.Platform == EPlatform::Unknown) return Lyrics(ELyricsSource::YouTube);

        return _yt.GetLyrics(searchResult.Data.Id);
    }

    return _yt.GetLyrics(track.Id);
}