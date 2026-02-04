#ifndef SPOTIFYDOWNLOADER_H
#define SPOTIFYDOWNLOADER_H

#include "IPlatformDownloader.h"

class SpotifyDownloader : public IPlatformDownloader
{
    private:
        ELinkType GetLinkType(const std::string& url) override;

        bool DownloadTrack(const std::string& url, const std::string& directory) override;
        bool DownloadPlaylist(const std::string& url, const std::string& directory) override { return false; }
        bool DownloadAlbum(const std::string& url, const std::string& directory)    override { return false; }
        bool DownloadEpisode(const std::string& url, const std::string& directory)  override { return false; }
};

#endif