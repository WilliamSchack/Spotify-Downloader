#include "SpotifyDownloader.h"

SpotifyDownloader::SpotifyDownloader() :_spotify() {}

ELinkType SpotifyDownloader::GetLinkType(const std::string& url)
{
    if (url.find("track") != std::string::npos)
        return ELinkType::Track;

    if (url.find("playlist") != std::string::npos)
        return ELinkType::Playlist;

    if (url.find("album") != std::string::npos)
        return ELinkType::Album;

    return ELinkType::Unknown;
}

std::string SpotifyDownloader::GetLinkId(const std::string& url)
{
    unsigned short startIndex = url.find_last_of('/') + 1;
    unsigned short endIndex = url.find('?');
    return url.substr(startIndex, endIndex - startIndex);
}

TrackData SpotifyDownloader::GetTrack(const std::string& url)
{
    std::string trackId = GetLinkId(url);
    if (trackId.empty()) return TrackData();

    if (url.find("episode") != std::string::npos)
        return _spotify.GetEpisode(trackId);
    
    return _spotify.GetTrack(trackId);
}