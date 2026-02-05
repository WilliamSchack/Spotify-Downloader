#ifndef IPLATFORMDOWNLOADER_H
#define IPLATFORMDOWNLOADER_H

#include "ELinkType.h"

#include "TrackData.h"
#include "PlaylistData.h"
#include "AlbumData.h"
#include "ArtistData.h"

#include <iostream>
#include <string>
#include <filesystem>

class IPlatformDownloader
{
    public:
        bool Download(const std::string& url, const std::string& directory);
    protected:
        virtual ELinkType GetLinkType(const std::string& url) = 0;

        virtual TrackData GetTrack(const std::string& url) = 0;
        virtual TrackData GetEpisode(const std::string& url) = 0;
        virtual PlaylistData GetPlaylist(const std::string& url) = 0;
        virtual AlbumData GetAlbum(const std::string& url) = 0;
    private:
        bool DownloadTrack(const std::string& url, const std::string& directory);
        bool DownloadEpisode(const std::string& url, const std::string& directory);
        bool DownloadPlaylist(const std::string& url, const std::string& directory);
        bool DownloadAlbum(const std::string& url, const std::string& directory);
};

#endif