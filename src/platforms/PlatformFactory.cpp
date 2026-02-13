#include "PlatformFactory.h"

std::unique_ptr<IPlatformDownloader> PlatformFactory::Create(const EPlatform& platform)
{
    switch (platform) {
        case EPlatform::Spotify:
            return std::make_unique<SpotifyDownloader>();
    }

    return nullptr;
}