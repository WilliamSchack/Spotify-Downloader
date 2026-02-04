#include "PlatformFactory.h"

std::unique_ptr<IPlatform> PlatformFactory::Create(EPlatform platform)
{
    switch (platform) {
        case EPlatform::Spotify:
            return std::make_unique<SpotifyPlatform>();
    }

    return nullptr;
}