#ifndef IPLATFORMDOWNLOADER_H
#define IPLATFORMDOWNLOADER_H

#include "IPlatformSearcher.h"
#include "PlatformUtils.h"
#include "LyricsFinder.h"
#include "Ytdlp.h"
#include "Ffmpeg.h"
#include "SpotifyAPI.h"
#include "FileUtils.h"
#include "ImageHandler.h"
#include "Config.h"
#include "CodecFactory.h"
#include "MetadataManager.h"
#include "ELinkType.h"

#include <iostream>

class IPlatformDownloader
{
    public:
        virtual ~IPlatformDownloader() = default;

        //bool Download(const std::string& url, const std::string& directory);

        virtual ELinkType GetLinkType(const std::string& url) = 0;
        virtual TrackData GetTrack(const std::string& url) = 0;
        virtual PlaylistTracks GetPlaylist(const std::string& url) = 0;
        virtual AlbumTracks GetAlbum(const std::string& url) = 0;

        bool DownloadTrack(const TrackData& track, const std::string& directory);
        int DownloadTracks(const std::vector<TrackData>& tracks, const std::string& directory);
    protected:
        virtual std::unique_ptr<IPlatformSearcher> GetSearcher() = 0;

    //private:
        //bool DownloadTrack(const std::string& url, const std::string& directory);
        //bool DownloadPlaylist(const std::string& url, const std::string& directory);
        //bool DownloadAlbum(const std::string& url, const std::string& directory);
    private:
        static std::filesystem::path FindAvailableTrackPath(const TrackData& track, const std::filesystem::path& originalPath);
    private:
        static inline const std::string DOWNLOADS_FOLDER_NAME = "Downloads";
        static inline const std::string IMAGES_FOLDER_NAME = "CoverArt";
};

#endif