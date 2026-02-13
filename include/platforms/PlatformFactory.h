#ifndef PLATFORMFACTORY_H
#define PLATFORMFACTORY_H

#include "EPlatform.h"
#include "SpotifyDownloader.h"

#include <memory>

class PlatformFactory
{
    public:
        static std::unique_ptr<IPlatformDownloader> Create(const EPlatform& platform);
};

#endif