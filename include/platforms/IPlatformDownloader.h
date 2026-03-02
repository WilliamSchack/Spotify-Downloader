#ifndef IPLATFORMDOWNLOADER_H
#define IPLATFORMDOWNLOADER_H

#include "ELinkType.h"
#include "EPlatform.h"
#include "TrackData.h"
#include "PlaylistTracks.h"
#include "AlbumTracks.h"

class IPlatformDownloader
{
    public:
        virtual ~IPlatformDownloader() = default;

        virtual ELinkType GetLinkType(const std::string& url) = 0;
        virtual EPlatform GetSearchPlatform() = 0;

        virtual TrackData GetTrack(const std::string& url) = 0;
        virtual PlaylistTracks GetPlaylist(const std::string& url) = 0;
        virtual AlbumTracks GetAlbum(const std::string& url) = 0;
};

#endif