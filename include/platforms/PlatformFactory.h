#ifndef PLATFORMFACTORY_H
#define PLATFORMFACTORY_H

#include "EPlatform.h"
#include "SpotifyDownloader.h"
#include "YoutubeSearcher.h"

#include <memory>

class PlatformFactory
{
    public:
        static std::unique_ptr<IPlatformDownloader> CreateDownloader(const EPlatform& platform);
        static std::unique_ptr<IPlatformSearcher> CreateSearcher(const EPlatform& platform);
};

#endif