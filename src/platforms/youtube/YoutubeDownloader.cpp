#include "YoutubeDownloader.h"

YoutubeDownloader::YoutubeDownloader() : _youtube() {}

std::string YoutubeDownloader::ProcessUrl(const std::string& url)
{
    // Convert youtu.be links
    std::string processedUrl = url;
    if (StringUtils::Contains(url, "youtu.be")) {
        size_t videoIdStartIndex = url.find_last_of("/") + 1;
        std::string videoId = url.substr(videoIdStartIndex);
        processedUrl = "https://www.youtube.com/watch?v=" + videoId;
    }

    return processedUrl;
}

ELinkType YoutubeDownloader::GetLinkType(const std::string& url)
{
    std::string processedUrl = ProcessUrl(url);

    if (StringUtils::Contains(processedUrl, "watch"))
        return ELinkType::Track;

    if (StringUtils::Contains(processedUrl, "playlist") && StringUtils::Contains(processedUrl, "OLAK"))
        return ELinkType::Album;

    if (StringUtils::Contains(processedUrl, "playlist"))
        return ELinkType::Playlist;

    return ELinkType::Unknown;
}

std::string YoutubeDownloader::GetLinkId(const std::string& url, const std::string& urlParam)
{
    std::string processedUrl = ProcessUrl(url);

    size_t startIndex = processedUrl.find("?" + urlParam + "=") + urlParam.size() + 2;
    size_t endIndex = processedUrl.find("&", startIndex);

    if (endIndex == std::string::npos)
        return processedUrl.substr(startIndex);

    return processedUrl.substr(startIndex, endIndex - startIndex);
}

EPlatform YoutubeDownloader::GetSearchPlatform()
{
    return EPlatform::YouTube;
}

TrackData YoutubeDownloader::GetTrack(const std::string& url)
{
    std::string id = GetLinkId(url, "v");
    std::cout << id << std::endl;
    if (id.empty()) return TrackData(EPlatform::Unknown);

    return _youtube.GetTrack(id);
}

PlaylistTracks YoutubeDownloader::GetPlaylist(const std::string& url)
{
    std::string id = GetLinkId(url, "list");
    if (id.empty()) return PlaylistTracks();

    return _youtube.GetPlaylist(id);
}

AlbumTracks YoutubeDownloader::GetAlbum(const std::string& url)
{
    std::string id = GetLinkId(url, "list");
    if (id.empty()) return AlbumTracks();

    std::string browseId = _youtube.GetAlbumBrowseId(id);
    if (browseId.empty()) return AlbumTracks();
    
    return _youtube.GetAlbum(browseId);
}