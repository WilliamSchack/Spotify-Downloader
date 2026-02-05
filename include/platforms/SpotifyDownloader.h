#ifndef SPOTIFYDOWNLOADER_H
#define SPOTIFYDOWNLOADER_H

#include "IPlatformDownloader.h"
#include "SpotifyAPI.h"

class SpotifyDownloader : public IPlatformDownloader
{
    public:
        SpotifyDownloader();
    private:
        SpotifyAPI _spotify;
    private:
        ELinkType GetLinkType(const std::string& url) override;
        std::string GetLinkId(const std::string& url);

        TrackData GetTrack(const std::string& url)       override;
        TrackData GetEpisode(const std::string& url)     override { return TrackData(); }
        PlaylistData GetPlaylist(const std::string& url) override { return PlaylistData(); }
        AlbumData GetAlbum(const std::string& url)       override { return AlbumData(); }
};

#endif