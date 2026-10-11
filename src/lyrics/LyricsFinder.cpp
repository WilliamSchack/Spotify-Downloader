#include "LyricsFinder.h"

Lyrics LyricsFinder::GetLyrics(const TrackData& track, const EPlatform& platform)
{
    std::unique_ptr<ILyricsPlatform> lyricsPlatform = LyricsPlatformFactory::Create(platform);
    if (lyricsPlatform == nullptr) return Lyrics();
    
    return lyricsPlatform->GetLyrics(track);
}

Lyrics LyricsFinder::GetSourceLyrics(const TrackData& track)
{
    return GetLyrics(track, track.Platform);
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

    return Lyrics(ELyricsSource::Unknown);
}