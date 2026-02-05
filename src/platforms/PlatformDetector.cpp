#include "PlatformDetector.h"

#include <iostream>

EPlatform PlatformDetector::GetPlatformFromUrl(const std::string& url)
{
    if (StringUtils::Contains(url, "open.spotify.com"))
        return EPlatform::Spotify;

    return EPlatform::Unknown;
}