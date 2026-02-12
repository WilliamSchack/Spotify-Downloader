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
    private:
        SpotifyAPI _spotify;
    private:
        ELinkType GetLinkType(const std::string& url) override;
        std::string GetLinkId(const std::string& url);

        std::unique_ptr<IPlatformSearcher> GetSearcher() override;

        TrackData GetTrack(const std::string& url)       override;
        PlaylistData GetPlaylist(const std::string& url) override { return PlaylistData(EPlatform::Unknown); }
        AlbumData GetAlbum(const std::string& url)       override { return AlbumData(EPlatform::Unknown); }
};

#endif