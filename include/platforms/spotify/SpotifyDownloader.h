#ifndef SPOTIFYDOWNLOADER_H
#define SPOTIFYDOWNLOADER_H

#include "IPlatformDownloader.h"
#include "YoutubeSearcher.h"
#include "SpotifyAPI.h"
#include "StringUtils.h"

class SpotifyDownloader : public IPlatformDownloader
{
    public:
        SpotifyDownloader();

        ELinkType GetLinkType(const std::string& url) override;
        EPlatform GetSearchPlatform() override;
    
        TrackData GetTrack(const std::string& url) override;
        PlaylistTracks GetPlaylist(const std::string& url) override;
        AlbumTracks GetAlbum(const std::string& url) override;
    private:
        std::string GetLinkId(const std::string& url);
    private:
        SpotifyAPI _spotify;
};

#endif