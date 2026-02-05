#include "SpotifyDownloader.h"

SpotifyDownloader::SpotifyDownloader() :_spotify() {}

ELinkType SpotifyDownloader::GetLinkType(const std::string& url)
{
    if (url.find("track") != std::string::npos)
        return ELinkType::Track;

    if (url.find("episode") != std::string::npos)
        return ELinkType::Episode;

    if (url.find("playlist") != std::string::npos)
        return ELinkType::Playlist;

    if (url.find("album") != std::string::npos)
        return ELinkType::Album;

    return ELinkType::Unknown;
}

std::string SpotifyDownloader::GetLinkId(const std::string& url)
{
    int startIndex = url.find_last_of('/') + 1;
    int endIndex = url.find('?');
    return url.substr(startIndex, endIndex - startIndex);
}

bool SpotifyDownloader::DownloadTrack(const std::string& url, const std::string& directory)
{
    std::string trackId = GetLinkId(url);
    TrackData track = _spotify.GetTrack(trackId);

    return true;
}