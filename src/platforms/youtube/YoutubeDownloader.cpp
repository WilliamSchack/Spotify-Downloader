#include "YoutubeDownloader.h"

YoutubeDownloader::YoutubeDownloader() : _youtube() {}

ELinkType YoutubeDownloader::GetLinkType(const std::string& url)
{
    if (StringUtils::Contains(url, "playlist") && StringUtils::Contains(url, "OLAK"))
        return ELinkType::Album;

    return ELinkType::Unknown;
}

std::string YoutubeDownloader::GetLinkId(const std::string& url)
{
    size_t startIndex = url.find("?list=") + 6;
    size_t endIndex = url.find("&", startIndex);

    if (endIndex == std::string::npos)
        return url.substr(startIndex);

    return url.substr(startIndex, endIndex - startIndex);
}

EPlatform YoutubeDownloader::GetSearchPlatform()
{
    return EPlatform::YouTube;
}

TrackData YoutubeDownloader::GetTrack(const std::string& url)
{
    return TrackData(EPlatform::Unknown);
}

PlaylistTracks YoutubeDownloader::GetPlaylist(const std::string& url)
{
    return PlaylistTracks();
}

AlbumTracks YoutubeDownloader::GetAlbum(const std::string& url)
{
    std::string id = GetLinkId(url);
    if (id.empty()) return AlbumTracks();

    std::string browseId = _youtube.GetAlbumBrowseId(id);
    if (browseId.empty()) return AlbumTracks();
    
    return _youtube.GetAlbum(browseId);
}