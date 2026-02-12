#ifndef IPLATFORMDOWNLOADER_H
#define IPLATFORMDOWNLOADER_H

#include "IPlatformSearcher.h"
#include "SpotifyAPI.h"
#include "FileUtils.h"
#include "ImageHandler.h"

#include "ELinkType.h"

#include <iostream>
#include <string>
#include <regex>
#include <filesystem>

class IPlatformDownloader
{
    public:
        virtual ~IPlatformDownloader() = default;

        bool Download(const std::string& url, const std::string& directory);
    protected:
        virtual ELinkType GetLinkType(const std::string& url) = 0;

        virtual std::unique_ptr<IPlatformSearcher> GetSearcher() = 0;

        virtual TrackData GetTrack(const std::string& url) = 0;
        virtual PlaylistData GetPlaylist(const std::string& url) = 0;
        virtual AlbumData GetAlbum(const std::string& url) = 0;
    private:
        static inline const std::string DOWNLOADS_FOLDER_NAME = "Downloads";
        static inline const std::string IMAGES_FOLDER_NAME = "CoverArt";
    private:
        bool DownloadTrack(const std::string& url, const std::string& directory);
        bool DownloadPlaylist(const std::string& url, const std::string& directory);
        bool DownloadAlbum(const std::string& url, const std::string& directory);
};

#endif