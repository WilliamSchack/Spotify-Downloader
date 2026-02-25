#include "LyricsFinder.h"

Lyrics LyricsFinder::GetLyrics(const TrackData& track, const ELyricsType& lyricsType)
{
    return Lyrics();
}

Lyrics LyricsFinder::GetSourceLyrics(const TrackData& source)
{
    std::unique_ptr<ILyricsPlatform> lyricsPlatform = LyricsPlatformFactory::Create(source.Platform);
    if (lyricsPlatform == nullptr) return Lyrics();
    
    return lyricsPlatform->GetLyrics(source);
}

Lyrics LyricsFinder::GetBestLyrics(const TrackData& track)
{
    return Lyrics();
}