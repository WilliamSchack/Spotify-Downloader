#include "PlatformFactory.h"

std::unique_ptr<IPlatformDownloader> PlatformFactory::CreateDownloader(const EPlatform& platform)
{
    switch (platform) {
        case EPlatform::Spotify: return std::make_unique<SpotifyDownloader>();
    }

    return nullptr;
}

std::unique_ptr<IPlatformSearcher> PlatformFactory::CreateSearcher(const EPlatform& platform)
{
    switch (platform) {
        case EPlatform::YouTube: return std::make_unique<YoutubeSearcher>();
    }

    return nullptr;
}