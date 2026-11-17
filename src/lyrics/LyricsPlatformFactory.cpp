#include "LyricsPlatformFactory.h"

std::unique_ptr<ILyricsPlatform> LyricsPlatformFactory::Create(const ELyricsSource& source)
{
    switch (source) {
        case ELyricsSource::YouTube: return std::make_unique<YoutubeLyrics>();
    }

    return nullptr;
}

std::unique_ptr<ILyricsPlatform> LyricsPlatformFactory::Create(const EPlatform& platform)
{
    switch (platform) {
        case EPlatform::Spotify: return Create(ELyricsSource::Musixmatch);
        case EPlatform::YouTube: return Create(ELyricsSource::YouTube);
    }

    return nullptr;
}