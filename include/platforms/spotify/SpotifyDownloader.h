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
        ELinkType GetLinkType(const std::string& url) override;
        std::string GetLinkId(const std::string& url);

        std::unique_ptr<IPlatformSearcher> GetSearcher() override;

        TrackData GetTrack(const std::string& url) override;
        PlaylistTracks GetPlaylist(const std::string& url) override;
        AlbumTracks GetAlbum(const std::string& url) override;
    private:
        SpotifyAPI _spotify;
};

#endif