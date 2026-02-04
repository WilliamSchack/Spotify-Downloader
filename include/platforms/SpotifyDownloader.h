#ifndef SPOTIFYDOWNLOADER_H
#define SPOTIFYDOWNLOADER_H

#include "IPlatformDownloader.h"

class SpotifyDownloader : public IPlatformDownloader
{
    private:
        ELinkType GetLinkType(const std::string& url) override;

        bool DownloadTrack(const std::string& url, const std::string& directory) override;
};

#endif