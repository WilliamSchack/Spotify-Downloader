#include "PlatformDetector.h"

#include <iostream>

EPlatform PlatformDetector::GetPlatformFromUrl(const std::string& url)
{
    if (url.find("open.spotify.com") != std::string::npos)
        return EPlatform::Spotify;

    return EPlatform::Unknown;
}