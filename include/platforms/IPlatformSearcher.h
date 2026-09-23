#ifndef IPLATFORMSEARCHER_H
#define IPLATFORMSEARCHER_H

#include "PlatformSearcherResult.h"
#include "TrackData.h"
#include "AlbumTracks.h"
#include "PlaylistTracks.h"

#include <functional>

// Gets track data and finds the same track(s) on another platform
class IPlatformSearcher
{
    public:
        virtual ~IPlatformSearcher() = default;

        virtual PlatformSearcherResult FindTrack(const TrackData& track, std::function<void(float)> progressCallback = nullptr) = 0;
        virtual AlbumTracks FindAlbum(const AlbumTracks& track) = 0;
        virtual PlaylistTracks FindPlaylist(const PlaylistTracks& track) = 0;
};

#endif