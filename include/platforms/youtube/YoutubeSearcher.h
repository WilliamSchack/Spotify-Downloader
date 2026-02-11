#ifndef YOUTUBESEARCHER_H
#define YOUTUBESEARCHER_H

#include "IPlatformSearcher.h"
#include "YTMusicAPI.h"
#include "ArrayUtils.h"

#include <regex>

class YoutubeSearcher : public IPlatformSearcher
{
    public:
        YoutubeSearcher();

        TrackData FindTrack(const TrackData& track) override;
        AlbumTracks FindAlbum(const AlbumTracks& track) override { return AlbumTracks(); };
        PlaylistTracks FindPlaylist(const PlaylistTracks& track) override { return PlaylistTracks(); };
    private:
        YTMusicAPI _yt;
    private:
        double ScoreTrack(const TrackData& src, const TrackData& track);
        
};

#endif