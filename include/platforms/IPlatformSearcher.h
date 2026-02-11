#ifndef IPLATFORMSEARCHER_H
#define IPLATFORMSEARCHER_H

#include "TrackData.h"
#include "AlbumTracks.h"
#include "PlaylistTracks.h"

// Gets track data and finds the same track(s) on another platform
class IPlatformSearcher
{
    public:
        virtual ~IPlatformSearcher() = default;

        virtual TrackData FindTrack(const TrackData& track) = 0;
        virtual AlbumTracks FindAlbum(const AlbumTracks& track) = 0;
        virtual PlaylistTracks FindPlaylist(const PlaylistTracks& track) = 0;
};

#endif