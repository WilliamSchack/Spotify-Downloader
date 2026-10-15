#ifndef YOUTUBEDATA_H
#define YOUTUBEDATA_H

#include "IPlatformData.h"
#include "YouTubeCookieFile.h"
#include "YTMusicAPI.h"
#include "Config.h"

class YoutubeData : public IPlatformData
{
    public:
        YoutubeData();

        std::string GetTrackDownloadError(const TrackData& track) override;

        std::vector<std::string> GetExtraDownloadArgs(const TrackData& track) override;

        std::string UpdateUrl(const TrackData& track) override;
    private:
        std::filesystem::path GetCookiePath();
        bool AgeRestricted(const std::string& trackId);

        bool HasPremium();
    private:
        YTMusicAPI _youtube;

        bool _premiumCacheSet;
        bool _premiumCache;
};

#endif