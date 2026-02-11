#ifndef YOUTUBESEARCHER_H
#define YOUTUBESEARCHER_H

#include "IPlatformSearcher.h"

class YoutubeSearcher : public IPlatformSearcher
{
    public:
        TrackData FindTrack(const TrackData& track) override;
        AlbumTracks FindAlbum(const AlbumTracks& track) override { return AlbumTracks(); };
        PlaylistTracks FindPlaylist(const PlaylistTracks& track) override { return PlaylistTracks() };
};

#endif