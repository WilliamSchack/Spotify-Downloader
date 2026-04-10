#ifndef YOUTUBEDOWNLOADER_H
#define YOUTUBEDOWNLOADER_H

#include "IPlatformDownloader.h"
#include "YTMusicAPI.h"
#include "StringUtils.h"

class YoutubeDownloader : public IPlatformDownloader
{
    public:
        YoutubeDownloader();

        ELinkType GetLinkType(const std::string& url) override;
        EPlatform GetSearchPlatform() override;
    
        TrackData GetTrack(const std::string& url) override;
        PlaylistTracks GetPlaylist(const std::string& url) override;
        AlbumTracks GetAlbum(const std::string& url) override;
    private:
        std::string GetLinkId(const std::string& url, const std::string& urlParam);
    private:
        YTMusicAPI _youtube;
};

#endif