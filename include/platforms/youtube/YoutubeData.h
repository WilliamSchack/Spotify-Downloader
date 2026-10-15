#ifndef YOUTUBEDATA_H
#define YOUTUBEDATA_H

#include "IPlatformData.h"
#include "YouTubeCookieFile.h"
#include "YTMusicAPI.h"

class YoutubeData : public IPlatformData
{
    public:
        YoutubeData();

        std::filesystem::path GetCookiePath() const override;
    private:
        YTMusicAPI _youtube;
};

#endif