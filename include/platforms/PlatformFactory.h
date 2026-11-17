#ifndef PLATFORMFACTORY_H
#define PLATFORMFACTORY_H

#include "EPlatform.h"
#include "SpotifyDownloader.h"
#include "YoutubeDownloader.h"
#include "YoutubeSearcher.h"
#include "YoutubeData.h"

#include <memory>

class PlatformFactory
{
    public:
        static std::unique_ptr<IPlatformDownloader> CreateDownloader(const EPlatform& platform);
        static std::unique_ptr<IPlatformSearcher> CreateSearcher(const EPlatform& platform);
        static std::unique_ptr<IPlatformData> CreateData(const EPlatform& platform);
};

#endif