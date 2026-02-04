#ifndef PLATFORMFACTORY_H
#define PLATFORMFACTORY_H

#include <memory>

#include "EPlatform.h"
#include "SpotifyDownloader.h"

class PlatformFactory
{
    public:
        static std::unique_ptr<IPlatformDownloader> Create(EPlatform platform);
};

#endif