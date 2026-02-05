#ifndef SPOTIFYDOWNLOADER_H
#define SPOTIFYDOWNLOADER_H

#include "IPlatformDownloader.h"
#include "SpotifyAPI.h"
#include "TrackData.h"

class SpotifyDownloader : public IPlatformDownloader
{
    public:
        SpotifyDownloader();
    private:
        SpotifyAPI _spotify;
    private:
        ELinkType GetLinkType(const std::string& url) override;
        std::string GetLinkId(const std::string& url);

        bool DownloadTrack(const std::string& url, const std::string& directory) override;
        bool DownloadPlaylist(const std::string& url, const std::string& directory) override { return false; }
        bool DownloadAlbum(const std::string& url, const std::string& directory)    override { return false; }
        bool DownloadEpisode(const std::string& url, const std::string& directory)  override { return false; }
};

#endif