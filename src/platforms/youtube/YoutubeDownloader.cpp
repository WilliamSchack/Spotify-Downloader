#include "YoutubeDownloader.h"

YoutubeDownloader::YoutubeDownloader() : _youtube() {}

ELinkType YoutubeDownloader::GetLinkType(const std::string& url)
{
    if (StringUtils::Contains(url, "watch"))
        return ELinkType::Track;

    if (StringUtils::Contains(url, "playlist") && StringUtils::Contains(url, "OLAK"))
        return ELinkType::Album;

    return ELinkType::Unknown;
}

std::string YoutubeDownloader::GetLinkId(const std::string& url, const std::string& urlParam)
{
    size_t startIndex = url.find("?" + urlParam + "=") + urlParam.size() + 2;
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
    std::string id = GetLinkId(url, "v");
    if (id.empty()) return TrackData(EPlatform::Unknown);

    return _youtube.GetTrack(id);
}

PlaylistTracks YoutubeDownloader::GetPlaylist(const std::string& url)
{
    return PlaylistTracks();
}

AlbumTracks YoutubeDownloader::GetAlbum(const std::string& url)
{
    std::string id = GetLinkId(url, "list");
    if (id.empty()) return AlbumTracks();

    std::string browseId = _youtube.GetAlbumBrowseId(id);
    if (browseId.empty()) return AlbumTracks();
    
    return _youtube.GetAlbum(browseId);
}