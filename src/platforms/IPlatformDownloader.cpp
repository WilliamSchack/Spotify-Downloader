#include "IPlatformDownloader.h"

bool IPlatformDownloader::Download(const std::string& url, const std::string& directory)
{
    std::cout << "Downloading..." << std::endl;

    ELinkType linkType = GetLinkType(url);
    if (linkType == ELinkType::Unknown)
        return false;

    bool directoryValid = std::filesystem::exists(directory);
    if (!directoryValid)
        return false;

    switch (linkType) {
        case ELinkType::Track:    return DownloadTrack(url, directory);
        case ELinkType::Playlist: return DownloadPlaylist(url, directory);
        case ELinkType::Album:    return DownloadAlbum(url, directory);
    }

    return false;
}

bool IPlatformDownloader::DownloadTrack(const std::string& url, const std::string& directory)
{
    TrackData track = GetTrack(url);
    return false;
}

bool IPlatformDownloader::DownloadPlaylist(const std::string& url, const std::string& directory)
{
    PlaylistData playlist = GetPlaylist(url);
    return false;
}   

bool IPlatformDownloader::DownloadAlbum(const std::string& url, const std::string& directory)
{
    AlbumData album = GetAlbum(url);
    return false;
}