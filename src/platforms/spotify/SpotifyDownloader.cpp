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

EPlatform SpotifyDownloader::GetSearchPlatform()
{
    return EPlatform::YouTube;
}

TrackData SpotifyDownloader::GetTrack(const std::string& url)
{
    std::string id = GetLinkId(url);
    if (id.empty()) return TrackData(EPlatform::Unknown);

    if (StringUtils::Contains(url, "episode"))
        return _spotify.GetEpisode(id);
    
    return _spotify.GetTrack(id);
}

PlaylistTracks SpotifyDownloader::GetPlaylist(const std::string& url)
{
    std::string id = GetLinkId(url);
    if (id.empty()) return PlaylistTracks();

    return _spotify.GetPlaylist(id);
}

AlbumTracks SpotifyDownloader::GetAlbum(const std::string& url)
{
    std::string id = GetLinkId(url);
    if (id.empty()) return AlbumTracks();

    return _spotify.GetAlbum(id);
}