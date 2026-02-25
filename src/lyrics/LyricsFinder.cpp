#include "LyricsFinder.h"

Lyrics LyricsFinder::GetSourceLyrics(const TrackData& track)
{
    std::unique_ptr<ILyricsPlatform> lyricsPlatform = LyricsPlatformFactory::Create(track.Platform);
    if (lyricsPlatform == nullptr) return Lyrics();
    
    return lyricsPlatform->GetLyrics(track);
}

Lyrics LyricsFinder::GetBestLyrics(const TrackData& track)
{
    // Loop through priority, if any is found, return that
    for (ELyricsSource source : Config::LYRICS_SOURCE_PRIORITY) {
        std::unique_ptr<ILyricsPlatform> lyricsPlatform = LyricsPlatformFactory::Create(source);
        if (lyricsPlatform == nullptr) continue;

        Lyrics lyrics = lyricsPlatform->GetLyrics(track);
        if (lyrics.Type == ELyricsType::None) continue;

        return lyrics;
    }

    return Lyrics();
}