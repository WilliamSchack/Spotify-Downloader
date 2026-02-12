#include "SpotifyDownloader.h"

SpotifyDownloader::SpotifyDownloader() : _spotify() {}

ELinkType SpotifyDownloader::GetLinkType(const std::string& url)
{
    if (StringUtils::Contains(url, "track"))
        return ELinkType::Track;

    if (StringUtils::Contains(url, "playlist"))
        return ELinkType::Playlist;

    if (StringUtils::Contains(url, "album"))
        return ELinkType::Album;

    return ELinkType::Unknown;
}

std::string SpotifyDownloader::GetLinkId(const std::string& url)
{
    unsigned short startIndex = url.find_last_of('/') + 1;
    unsigned short endIndex = url.find('?');
    return url.substr(startIndex, endIndex - startIndex);
}

std::unique_ptr<IPlatformSearcher> GetSearcher()
{
    return std::make_unique<YoutubeSearcher>();
}

TrackData SpotifyDownloader::GetTrack(const std::string& url)
{
    std::string trackId = GetLinkId(url);
    if (trackId.empty()) return TrackData(EPlatform::Unknown);

    if (StringUtils::Contains(url, "episode"))
        return _spotify.GetEpisode(trackId);
    
    return _spotify.GetTrack(trackId);
}