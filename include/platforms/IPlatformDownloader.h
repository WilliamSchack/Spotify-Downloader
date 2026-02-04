#ifndef IPLATFORMDOWNLOADER_H
#define IPLATFORMDOWNLOADER_H

#include <iostream>
#include <string>
#include <filesystem>

#include "ELinkType.h"

class IPlatformDownloader
{
    public:
        bool Download(const std::string& url, const std::string& directory);
    protected:
        virtual ELinkType GetLinkType(const std::string& url) = 0;

        virtual bool DownloadTrack(const std::string& url, const std::string& directory) = 0;
        virtual bool DownloadPlaylist(const std::string& url, const std::string& directory) = 0;
        virtual bool DownloadAlbum(const std::string& url, const std::string& directory) = 0;
        virtual bool DownloadEpisode(const std::string& url, const std::string& directory) = 0;
};

#endif