#include "PlatformUtils.h"

std::string PlatformUtils::GetPlatformString(EPlatform platform)
{
    switch (platform) {
        case EPlatform::YouTube: return "YouTube";
        case EPlatform::Spotify: return "Spotify";
    }

    return "Unknown";
}