#include "SpotifyDownloader.h"

ELinkType SpotifyDownloader::GetLinkType(const std::string& url)
{
    if (url.find("track") != std::string::npos)
        return ELinkType::Track;

    if (url.find("playlist") != std::string::npos)
        return ELinkType::Playlist;

    if (url.find("album") != std::string::npos)
        return ELinkType::Album;

    if (url.find("episode") != std::string::npos)
        return ELinkType::Episode;

    return ELinkType::Unknown;
}