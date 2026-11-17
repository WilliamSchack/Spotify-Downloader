#include "PlatformDetector.h"

#include <iostream>

EPlatform PlatformDetector::GetPlatformFromUrl(const std::string& url)
{
    if (StringUtils::Contains(url, "open.spotify.com"))
        return EPlatform::Spotify;

    if (StringUtils::Contains(url, "youtube.com") || StringUtils::Contains(url, "youtu.be"))
        return EPlatform::YouTube;

    return EPlatform::Unknown;
}